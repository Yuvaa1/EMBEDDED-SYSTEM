#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "driver/gpio.h"

#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "wifi station";

static int s_retry_num = 0;

static bool led_state = false;
#define LED_GPIO_PIN 21

#define URI_ROOT "/"
#define URI_LED_CONTROL "/led"
#define URI_LED_STATUS "/led/status"

static esp_err_t root_get_handler(httpd_req_t *req)
{
    const char *html_response =
        "<html>"
        "<head><title>ESP32 LED Control</title></head>"
        "<body>"
        "<h1>ESP32 WiFi LED Control</h1>"
        "<p>LED Status: <span id=\"led-status\">%s</span></p>"
        "<p><a href=\"/led?state=on\"><button style=\"background-color: green;\">Turn On</button></a></p>"
        "<p><a href=\"/led?state=off\"><button style=\"background-color: red;\">Turn Off</button></a></p>"
        "<script>"
        "   fetch('/led/status')"
        "       .then(response => response.text())"
        "       .then(status => document.getElementById('led-status').innerText = status);"
        "</script>"
        "</body>"
        "</html>";

    char led_status[16];
    snprintf(led_status, sizeof(led_status), "%s", led_state ? "ON" : "OFF");

    char formatted_html_response[512];
    snprintf(formatted_html_response, sizeof(formatted_html_response), html_response, led_status);

    httpd_resp_send(req, formatted_html_response, strlen(formatted_html_response));

    return ESP_OK;
}

static esp_err_t led_control_get_handler(httpd_req_t *req)
{
    char query[100];
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK)
    {
        char param[32];
        if (httpd_query_key_value(query, "state", param, sizeof(param)) == ESP_OK)
        {
            if (strcmp(param, "on") == 0)
            {
                gpio_set_level(LED_GPIO_PIN, 1);  // Turn on LED
                led_state = true;
            }
            else if (strcmp(param, "off") == 0)
            {
                gpio_set_level(LED_GPIO_PIN, 0);  // Turn off LED
                led_state = false;
            }
        }
    }

    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", URI_ROOT);
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}

static esp_err_t led_status_get_handler(httpd_req_t *req)
{
    char led_status[16];
    snprintf(led_status, sizeof(led_status), "%s", led_state ? "ON" : "OFF");

    httpd_resp_send(req, led_status, strlen(led_status));

    return ESP_OK;
}

httpd_uri_t root = {
    .uri = URI_ROOT,
    .method = HTTP_GET,
    .handler = root_get_handler,
    .user_ctx = NULL
};

httpd_uri_t led_control = {
    .uri = URI_LED_CONTROL,
    .method = HTTP_GET,
    .handler = led_control_get_handler,
    .user_ctx = NULL
};

httpd_uri_t led_status = {
    .uri = URI_LED_STATUS,
    .method = HTTP_GET,
    .handler = led_status_get_handler,
    .user_ctx = NULL
};

static void register_http_handlers(httpd_handle_t server)
{
    httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &led_control);
    httpd_register_uri_handler(server, &led_status);
}

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to AP SSID:%s", EXAMPLE_ESP_WIFI_SSID);
    }
}

void app_main(void)
{
    // Initialize NVS
   esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_rom_gpio_pad_select_gpio(LED_GPIO_PIN);
    gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK)
    {
        register_http_handlers(server);
    }
}
