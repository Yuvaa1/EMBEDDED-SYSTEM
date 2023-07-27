#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"
#include "esp_bt_device.h"
#include "nvs_flash.h"
#include "esp_log.h"

#define SPP_TAG "SPP_SERVER"
#define SPP_SERVER_NAME "ESP32 SPP Server"
#define SPP_SERVER_SCN 3  // RFCOMM channel number
#define APP_PARTITION_SIZE 0x200000 // 2 MB
// Function to adjust the partition table size
void adjust_partition_table_size()
{
    esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
    if (partition != NULL)
    {
        // Update the partition table size
        partition->size = APP_PARTITION_SIZE;

        // Commit the changes
        esp_partition_erase_range(partition, 0, partition->size);
        esp_partition_write(partition, 0, partition->label, partition->size);
    }
}

static void spp_server_event_handler(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event) {
        case ESP_SPP_INIT_EVT:
            esp_bt_dev_set_device_name(SPP_SERVER_NAME);
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
            esp_spp_start_srv(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, SPP_SERVER_SCN, SPP_SERVER_NAME);
            break;

        case ESP_SPP_DISCOVERY_COMP_EVT:
            ESP_LOGI(SPP_TAG, "Service discovery complete");
            break;

        case ESP_SPP_OPEN_EVT:
            ESP_LOGI(SPP_TAG, "Client connected");
            break;

        case ESP_SPP_CLOSE_EVT:
            ESP_LOGI(SPP_TAG, "Client disconnected");
            break;

        case ESP_SPP_START_EVT:
            ESP_LOGI(SPP_TAG, "SPP server started");
            break;

        case ESP_SPP_CL_INIT_EVT:
            ESP_LOGI(SPP_TAG, "Client initiated a connection");
            break;

        case ESP_SPP_DATA_IND_EVT:
            ESP_LOGI(SPP_TAG, "Received data from client: %.*s", param->data_ind.len, param->data_ind.data);
            break;

        default:
            break;
    }
}

static void spp_server_task(void *params)
{
    esp_err_t ret = esp_spp_register_callback(spp_server_event_handler);
    if (ret != ESP_OK) {
        ESP_LOGE(SPP_TAG, "Failed to register SPP callback: %d", ret);
        vTaskDelete(NULL);
        return;
    }

    ret = esp_spp_enhanced_init(ESP_SPP_MODE_CB);
    if (ret != ESP_OK) {
        ESP_LOGE(SPP_TAG, "Failed to initialize SPP: %d", ret);
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));  // Keep the task running
    }
}

void app_main()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGI(SPP_TAG, "Erasing NVS flash...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(SPP_TAG, "Bluetooth controller initialization failed: %d", ret);
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
    if (ret != ESP_OK) {
        ESP_LOGE(SPP_TAG, "Bluetooth controller enable failed: %d", ret);
        return;
    }

    ret = esp_bluedroid_init();
    if (ret != ESP_OK) {
        ESP_LOGE(SPP_TAG, "Bluedroid initialization failed: %d", ret);
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret != ESP_OK) {
        ESP_LOGE(SPP_TAG, "Bluedroid enable failed: %d", ret);
        return;
    }

    vTaskDelay(pdMS_TO_TICKS(2000));

    xTaskCreate(&spp_server_task, "spp_server_task", 8192, NULL, 10, NULL);
}

