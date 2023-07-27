#include <iostream>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_http_server.h"
#include "TinyGPSPlus.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include <sstream>

// Function declarations
httpd_handle_t start_webserver();
void gps_task(void* pvParameters);

TinyGPSPlus gps;
#define UART_NUM UART_NUM_2
#define UART_TX_PIN GPIO_NUM_17
#define UART_RX_PIN GPIO_NUM_16

// Rename the main function to app_main
void app_main()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize WiFi
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    // Initialize UART
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0, // Set your desired threshold here
        .source_clk = UART_SCLK_REF_TICK
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, 256, 0, 0, nullptr, 0);

    // Start the web server
    httpd_handle_t server = start_webserver();
    if (server == nullptr)
    {
        ESP_LOGE("TAG", "Failed to start web server");
        return;
    }
    std::cout << "Hello, ESP32!" << std::endl;

    // Create GPS task
    xTaskCreate(gps_task, "gps_task", 4096, nullptr, 5, nullptr);
}

void gps_task(void *pvParameters)
{
    float latitude, longitude;
    char gps_data[128];

    while (1)
    {
        if (uart_read_bytes(UART_NUM, (uint8_t *)gps_data, sizeof(gps_data), 1000 / portTICK_PERIOD_MS) > 0)
        {
            // Parse GPS data
            for (int i = 0; i < sizeof(gps_data); i++)
            {
                if (gps.encode(gps_data[i]))
                {
                    if (gps.location.isValid())
                    {
                        latitude = gps.location.lat();
                        longitude = gps.location.lng();
                        printf("Latitude: %.6f, Longitude: %.6f\n", latitude, longitude);
                    }
                    break;
                }
            }
        }
    }
}

static esp_err_t root_get_handler(httpd_req_t *req)
{
    float latitude, longitude;

    // Get GPS data
    latitude = gps.location.lat();
    longitude = gps.location.lng();

    std::stringstream html;
    html << "<!DOCTYPE html><html><body>"
         << "<h1>SIM28M (GPS RECEIVER) DATA</h1>"
         << "<p><b>Location Details</b></p>"
         << "<p>Latitude: " << latitude << "</p>"
         << "<p>Longitude: " << longitude << "</p>"
         << "</body></html>";

    std::string response = html.str();
    httpd_resp_send(req, response.c_str(), response.length());
    return ESP_OK;
}

httpd_handle_t start_webserver()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    // Start the httpd server
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Set URI handlers
        httpd_uri_t root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_get_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &root);
        return server;
    }

    return NULL;
}
