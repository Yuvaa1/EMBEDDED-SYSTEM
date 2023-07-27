#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_http_server.h"
#include "esp_event.h"

#define GPS_UART_NUM UART_NUM_2
#define GPS_UART_TX_PIN 17
#define GPS_UART_RX_PIN 16
#define GPS_UART_BAUD_RATE 9600
#define HTTP_RESPONSE_MAX_SIZE 1500
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

float latitude, longitude;
char date_str[12] = "DD/MM/YY";
char time_str[12] = "HH:mm:ss";

/* HTTP server handler to send GPS data */
esp_err_t gps_data_handler(httpd_req_t *req) {
    char response[HTTP_RESPONSE_MAX_SIZE];
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
        "<!DOCTYPE html><html><head><title>GPS DATA</title>"
        "<style>a:link{background-color:RED;text-decoration:none;}"
        "table, th, td {border: 1px solid black; border-collapse: collapse;}"
        "</style></head><body style=\"background-color:YELLOW;\">"
        "<h1 style=\"color:RED;font-size:300%%;text-align:center;\">SIM28M (GPS RECEIVER) DATA</h1>"
        "<p style=\"color:BLUE;font-size:250%%;text-align:center;\"><b>Location Details</b></p>"
        "<table style=\"font-size:150%%;margin:auto;\">"
        "<tr><th>Latitude :-</th><td style=\"text-align:center;\">%.6f</td></tr>"
        "<tr><th>Longitude :-</th><td style=\"text-align:center;\">%.6f</td></tr>"
        "<tr><th>Date :-</th><td style=\"text-align:center;\">%s</td></tr>"
        "<tr><th>Time :-</th><td style=\"text-align:center;\">%s</td></tr>"
        "</table></body></html>",
        latitude, longitude, date_str, time_str
    );
    httpd_resp_send(req, response, strlen(response));
    return ESP_OK;
}

httpd_uri_t gps_data_uri = {
    .uri = "/gps",
    .method = HTTP_GET,
    .handler = gps_data_handler,
    .user_ctx = NULL
};

void parse_nmea(char *nmea_sentence, float *latitude, float *longitude) {
    char *token;
    char *rest = nmea_sentence;
    int count = 0;

    // Split the NMEA sentence using ',' as the delimiter
    while ((token = strtok_r(rest, ",", &rest)) != NULL) {
        count++;
        if (count == 3) {
            *latitude = atof(token); // Convert the latitude string to a float
        } else if (count == 5) {
            *longitude = atof(token); // Convert the longitude string to a float
            break;
        }
    }
}

void gps_task(void *pvParameters) {
    char nmea_sentence[128] = {0};

    uart_config_t uart_config = {
        .baud_rate = GPS_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(GPS_UART_NUM, &uart_config);
    uart_set_pin(GPS_UART_NUM, GPS_UART_TX_PIN, GPS_UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(GPS_UART_NUM, 2048, 0, 0, NULL, 0);

    while(1) {
        // Read data from GPS module via UART
        int len = uart_read_bytes(GPS_UART_NUM, (uint8_t *)nmea_sentence, sizeof(nmea_sentence)-1, 100 / portTICK_PERIOD_MS);
        if (len > 0) {
            nmea_sentence[len] = '\0';
            // Check if the NMEA sentence starts with "$GPGGA"
            if (strncmp(nmea_sentence, "$GPGGA,", 7) == 0) {
                parse_nmea(nmea_sentence, &latitude, &longitude);
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void wifi_task(void *pvParameters) {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();

    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &gps_data_uri);
    }
    return server;
}

void stop_webserver(httpd_handle_t server) {
    httpd_stop(server);
}

void app_main() {
    httpd_handle_t server = NULL;

    xTaskCreate(gps_task, "gps_task", 4096, NULL, 5, NULL);
    xTaskCreate(wifi_task, "wifi_task", 4096, NULL, 4, NULL);

    server = start_webserver();

    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    if (server) {
        stop_webserver(server);
    }
}
SIM28M (GPS RECEIVER) DATA

Location Details
Latitude: XX.XXXXXX
Longitude: YY.YYYYYY
Date: YYYY-MM-DD
Time: HH:MM:SS
If the GPS module is not connected or if it fails to read GPS data, the displayed latitude and longitude values will remain unchanged. The date and time should still be provided by the ESP32 system clock.




