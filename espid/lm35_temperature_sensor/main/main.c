#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF    1100        // Default ADC reference voltage in mV
#define NO_OF_SAMPLES   64          // Number of ADC samples for smoothing
#define LM35_PIN        34          // GPIO pin number to which LM35 sensor is connected

void lm35_temperature_sensor_task(void *pvParameters)
{
    esp_adc_cal_characteristics_t adc_cal;
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_cal);
    
    while (1)
    {
        uint32_t adc_reading = 0;
        for (int i = 0; i < NO_OF_SAMPLES; i++)
        {
            adc_reading += adc1_get_raw(ADC1_CHANNEL_6);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        adc_reading /= NO_OF_SAMPLES;
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, &adc_cal);
        float temperature = (voltage - 500) / 10.0;

        printf("Temperature: %.2f°C\n", temperature);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    esp_err_t err = adc1_config_width(ADC_WIDTH_BIT_12);
    if (err != ESP_OK) {
        printf("ADC width configuration error: %d\n", err);
        return;
    }

    err = adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
    if (err != ESP_OK) {
        printf("ADC attenuation configuration error: %d\n", err);
        return;
    }

    xTaskCreate(lm35_temperature_sensor_task, "lm35_temperature_sensor_task", 4096, NULL, 5, NULL);
}

