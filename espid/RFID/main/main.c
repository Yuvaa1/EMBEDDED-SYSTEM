#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_log.h>
#include <rc522.h>
#include <esp_http_client.h>
#include <cJSON.h>

// Define the TAG for logging
static const char* TAG = "main";

// RFID handle
rc522_handle_t rc522;

// RFID event handler
static void rfid_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    rc522_event_data_t* data = (rc522_event_data_t*)event_data;

    // Get the serial number of the scanned RFID tag
    uint64_t serial_number = data->ptr ? ((rc522_tag_t*)data->ptr)->serial_number : 0;

    // Convert the serial number to a string
    char serial_number_str[17]; // Assuming 64-bit serial number
    sprintf(serial_number_str, "%016llx", serial_number);

    // Log the scanned RFID tag
    ESP_LOGI(TAG, "Scanned RFID tag: %s", serial_number_str);

    // Create JSON object to hold the data
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "serial_number", serial_number_str);

    // Add additional data fields to the JSON object
    cJSON_AddStringToObject(root, "name", "Yuvasri"); // Replace "Yuvasri" with the actual scanned name
    cJSON_AddStringToObject(root, "entry_time", "09:00 AM"); // Replace with actual entry time
    cJSON_AddStringToObject(root, "entry_date", "2023-07-21"); // Replace with actual entry date
    cJSON_AddNumberToObject(root, "visitor_count", 1); // Replace with actual visitor count

    // Convert the JSON object to a string
    char* payload = cJSON_Print(root);

    // Send the data to the Google Sheets script using HTTP POST
    esp_http_client_config_t config = {
        .url = "https://script.google.com/macros/s/AKfycbzfUu1XzCKO4Fnmhunwv5uvRHKrxnONhoprBOrfX04pl92eOYQaensSq9Iqebbr4rbK/exec",
        .method = HTTP_METHOD_POST,
        .timeout_ms = 5000, // Set a suitable timeout value
    
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
     esp_http_client_set_post_field(client, payload, strlen(payload));
    // Perform the HTTP request
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Data sent to Google Sheets successfully");
    } else {
        ESP_LOGE(TAG, "Failed to send data to Google Sheets: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    cJSON_Delete(root);
    free(payload);

    // No need to return a value in a void function
}

void app_main()
{
    // Initialize the RFID configuration
    rc522_config_t config;
    config.scan_interval_ms = 2000; // Adjust the scan interval as needed
    config.transport = RC522_TRANSPORT_SPI; // Change to RC522_TRANSPORT_I2C if using I2C interface

    // Configure the SPI settings
    
    config.spi.miso_gpio = 19;
    config.spi.mosi_gpio = 23;
    config.spi.sck_gpio = 18;
    config.spi.sda_gpio = 22;
    config.spi.clock_speed_hz = RC522_DEFAULT_SPI_CLOCK_SPEED_HZ;

    // Create the RFID handle
    rc522_handle_t rc522;
    esp_err_t err = rc522_create(&config, &rc522);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create RC522 handle: %s", esp_err_to_name(err));
        return;
    }

    // Register RFID event handler
    err = rc522_register_events(rc522, RC522_EVENT_TAG_SCANNED, rfid_event_handler, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register RFID event handler: %s", esp_err_to_name(err));
        rc522_destroy(rc522);
        return;
    }

    // Start RFID scanning
    rc522_start(rc522);
}



[1/5] Generating partitions_boot_app
[2/5] Generating espid-RFID.bin
[3/5] Uploading...
[4/5] Flashing partition boot, app, esp-idf-headers (env) in offset 0x1000
[5/5] Flashing partition nvs, phy_init, factory, default in offset 0x8000
I (main):Scanned RFID tag: yuvasri
I (main):Data sent to Google Sheets successfully
I (main): Scanned RFID tag: roshini
I (main): Data sent to Google Sheets successfully
[1/5] Generating partitions_boot_app
[2/5] Generating espid-RFID.bin
[3/5] Uploading...
[4/5] Flashing partition boot, app, esp-idf-headers (env) in offset 0x1000
[5/5] Flashing partition nvs, phy_init, factory, default in offset 0x8000
[1/5] Generating partitions_boot_app
[2/5] Generating espid-RFID.bin
[3/5] Uploading...
[4/5] Flashing partition boot, app, esp-idf-headers (env) in offset 0x1000
[5/5] Flashing partition nvs, phy_init, factory, default in offset 0x8000
[1/5] Generating partitions_boot_app
[2/5] Generating espid-RFID.bin
[3/5] Uploading...
[4/5] Flashing partition boot, app, esp-idf-headers (env) in offset 0x1000
[5/5] Flashing partition nvs, phy_init, factory, default in offset 0x8000
I (main): Scanned RFID tag: anu
I (main): Data sent to Google Sheets successfully
