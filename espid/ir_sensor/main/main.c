#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define PIR_PIN GPIO_NUM_27
#define LED_PIN GPIO_NUM_18

int visitor_count = 0;  // Global variable to keep track of visitor count

void motion_detected_task(void *pvParameters) {
    gpio_set_direction(PIR_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while (1) {
        if (gpio_get_level(PIR_PIN)) {
            gpio_set_level(LED_PIN, 1);  // Turn on the LED
            visitor_count++;  // Increment visitor count
            printf("Visitor count: %d\n", visitor_count);  // Display visitor count
            vTaskDelay(5000 / portTICK_PERIOD_MS);  // Delay for 5 seconds
        } else {
            gpio_set_level(LED_PIN, 0);  // Turn off the LED
            visitor_count = 0;  // Reset visitor count when no object is detected
            printf("No object detected\n");
            vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay for 100 milliseconds
        }
    }
}

void app_main() {
    xTaskCreate(motion_detected_task, "motion_task", 2048, NULL, 5, NULL);
}
