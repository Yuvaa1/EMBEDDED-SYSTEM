#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#define LED_PIN_1 GPIO_NUM_2
#define LED_PIN_2 GPIO_NUM_4

void blink_task(void *pvParameters)
{
    
    gpio_set_direction(LED_PIN_1, GPIO_MODE_OUTPUT);

    gpio_set_direction(LED_PIN_2, GPIO_MODE_OUTPUT);

    while (1)
    {
        // Blink LED 1 every 1 second
        gpio_set_level(LED_PIN_1, 1);
        printf("LED 1 is ON\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN_1, 0);
        printf("LED 1 is OFF\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // Blink LED 2 every 2 seconds
        gpio_set_level(LED_PIN_2, 1);
        printf("LED 2 is ON\n");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN_2, 0);
        printf("LED 2 is OFF\n");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    gpio_set_direction(LED_PIN_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_PIN_2, GPIO_MODE_OUTPUT);

    xTaskCreate(&blink_task, "blink_task", 4096, NULL, 5, NULL);
}
