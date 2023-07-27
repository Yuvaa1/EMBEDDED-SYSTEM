#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <esp_netif.h>
#include <esp_eth.h>
#include <esp_http_server.h>
#include <string.h>

#define LED_GPIO_PIN GPIO_NUM_21

static const char *TAG = "web_server";

esp_err_t led_on_handler(httpd_req_t *req)
{
    gpio_set_level(LED_GPIO_PIN, 1);
    httpd_resp_send(req, "LED is ON", strlen("LED is ON"));
    return ESP_OK;
}

esp_err_t led_off_handler(httpd_req_t *req)
{
    gpio_set_level(LED_GPIO_PIN, 0);
    httpd_resp_send(req, "LED is OFF", strlen("LED is OFF"));
    return ESP_OK;
}

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the HTTP Server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Register URI handlers
        httpd_uri_t led_on_uri = {
            .uri = "/led/on",
            .method = HTTP_GET,
            .handler = led_on_handler,
            .user_ctx = NULL
        };
        httpd_uri_t led_off_uri = {
            .uri = "/led/off",
            .method = HTTP_GET,
            .handler = led_off_handler,
            .user_ctx = NULL
        };

        httpd_register_uri_handler(server, &led_on_uri);
        httpd_register_uri_handler(server, &led_off_uri);
    }
    else
    {
        ESP_LOGE(TAG, "Error starting server!");
    }

    return server;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the HTTP Server
    httpd_stop(server);
}

void app_main()
{
    // Initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Initialize Wi-Fi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "your_ssid",       // Replace "your_ssid" with your actual SSID
            .password = "your_password",   // Replace "your_password" with your actual password
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Wait for Wi-Fi connection
    while (1)
    {
        uint8_t isConnected;
        wifi_ap_record_t ap_info;
        ESP_ERROR_CHECK(esp_wifi_sta_get_ap_info(&ap_info));
        isConnected = (ap_info.primary != 0);

        if (isConnected)
        {
            ESP_LOGI(TAG, "Connected to Wi-Fi");
            break;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    // Configure the LED GPIO pin
  
    gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT);

    // Start the web server
    httpd_handle_t server = start_webserver();
    if (server == NULL)
    {
        ESP_LOGE(TAG, "Failed to start the web server");
        return;
    }

    // Main loop
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    // Stop the web server
    stop_webserver(server);
}
