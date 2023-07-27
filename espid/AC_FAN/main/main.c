#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#define RELAY_PIN GPIO_NUM_27

void app_main(void)
{
    // Configure the GPIO pin for the relay
 
    gpio_set_direction(RELAY_PIN, GPIO_MODE_OUTPUT);

    // Turn on the AC fan
    gpio_set_level(RELAY_PIN, 1);
    printf("AC fan turned on\n");
    vTaskDelay(5000 / portTICK_PERIOD_MS); // Wait for 5 seconds

    // Turn off the AC fan
    gpio_set_level(RELAY_PIN, 0);
    printf("AC fan turned off\n");

    vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait for 1 second before restarting the task
    esp_restart(); // Restart the ESP32 to repeat the process
}

