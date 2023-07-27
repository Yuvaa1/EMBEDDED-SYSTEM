#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "driver/gpio.h"


#define STEPS_PER_REV 200
#define IN1_GPIO 14
#define IN2_GPIO 27
#define IN3_GPIO 26
#define IN4_GPIO 25

void stepper_task(void *pvParameters)
{
    gpio_set_direction(IN1_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN2_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN3_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(IN4_GPIO, GPIO_MODE_OUTPUT);

    int step_delay = 500; // Delay between steps in milliseconds
    int steps = STEPS_PER_REV; // Number of steps for one revolution

    while (1)
    {
        printf("Rotating Clockwise...\n");
        for (int i = 0; i < steps; i++)
        {
            // Step sequence for clockwise rotation
            gpio_set_level(IN1_GPIO, 1);
            gpio_set_level(IN2_GPIO, 0);
            gpio_set_level(IN3_GPIO, 1);
            gpio_set_level(IN4_GPIO, 0);
            vTaskDelay(pdMS_TO_TICKS(step_delay));
            gpio_set_level(IN1_GPIO, 0);
            gpio_set_level(IN2_GPIO, 1);
            gpio_set_level(IN3_GPIO, 1);
            gpio_set_level(IN4_GPIO, 0);
            vTaskDelay(pdMS_TO_TICKS(step_delay));
            gpio_set_level(IN1_GPIO, 0);
            gpio_set_level(IN2_GPIO, 1);
            gpio_set_level(IN3_GPIO, 0);
            gpio_set_level(IN4_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(step_delay));
            gpio_set_level(IN1_GPIO, 1);
            gpio_set_level(IN2_GPIO, 0);
            gpio_set_level(IN3_GPIO, 0);
            gpio_set_level(IN4_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(step_delay));
        }
        vTaskDelay(pdMS_TO_TICKS(500));

        printf("Rotating Anti-clockwise...\n");
        for (int i = 0; i < steps; i++)
        {
            // Step sequence for anti-clockwise rotation
            gpio_set_level(IN1_GPIO, 1);
            gpio_set_level(IN2_GPIO, 0);
            gpio_set_level(IN3_GPIO, 0);
            gpio_set_level(IN4_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(step_delay));
            gpio_set_level(IN1_GPIO, 0);
            gpio_set_level(IN2_GPIO, 1);
            gpio_set_level(IN3_GPIO, 0);
            gpio_set_level(IN4_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(step_delay));
            gpio_set_level(IN1_GPIO, 0);
            gpio_set_level(IN2_GPIO, 1);
            gpio_set_level(IN3_GPIO, 1);
            gpio_set_level(IN4_GPIO, 0);
            vTaskDelay(pdMS_TO_TICKS(step_delay));
            gpio_set_level(IN1_GPIO, 1);
            gpio_set_level(IN2_GPIO, 0);
            gpio_set_level(IN3_GPIO, 1);
            gpio_set_level(IN4_GPIO, 0);
            vTaskDelay(pdMS_TO_TICKS(step_delay));
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main()
{
    xTaskCreate(stepper_task, "stepper_task", 2048, NULL, 5, NULL);
}
