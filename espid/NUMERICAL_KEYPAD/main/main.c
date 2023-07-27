#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"
#include "sdkconfig.h"

#define ROW1_PIN GPIO_NUM_21
#define ROW2_PIN GPIO_NUM_19
#define ROW3_PIN GPIO_NUM_18
#define ROW4_PIN GPIO_NUM_5
#define COL1_PIN GPIO_NUM_12
#define COL2_PIN GPIO_NUM_13
#define COL3_PIN GPIO_NUM_14
#define COL4_PIN GPIO_NUM_15

void keypad_init();
int keypad_scan();

void app_main(void)
{
    keypad_init();

    while (1)
    {
        int key = keypad_scan();

        if (key != -1)
        {
            printf("Key Pressed: %d\n", key);
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void keypad_init()
{
    gpio_set_direction(ROW1_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ROW2_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ROW3_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ROW4_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(COL1_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(COL2_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(COL3_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(COL4_PIN, GPIO_MODE_INPUT);

    gpio_set_pull_mode(COL1_PIN, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(COL2_PIN, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(COL3_PIN, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(COL4_PIN, GPIO_PULLUP_ONLY);
}

int keypad_scan()
{
    int key = -1;

    // Set Row 1 Low and check columns
    gpio_set_level(ROW1_PIN, 0);
    if (gpio_get_level(COL1_PIN) == 0)
        key = 1;
    else if (gpio_get_level(COL2_PIN) == 0)
        key = 2;
    else if (gpio_get_level(COL3_PIN) == 0)
        key = 3;
    else if (gpio_get_level(COL4_PIN) == 0)
        key = 4;
    gpio_set_level(ROW1_PIN, 1);

    // Set Row 2 Low and check columns
    gpio_set_level(ROW2_PIN, 0);
    if (gpio_get_level(COL1_PIN) == 0)
        key = 5;
    else if (gpio_get_level(COL2_PIN) == 0)
        key = 6;
    else if (gpio_get_level(COL3_PIN) == 0)
        key = 7;
    else if (gpio_get_level(COL4_PIN) == 0)
        key = 8;
    gpio_set_level(ROW2_PIN, 1);

    // Set Row 3 Low and check columns
    gpio_set_level(ROW3_PIN, 0);
    if (gpio_get_level(COL1_PIN) == 0)
        key = 9;
    else if (gpio_get_level(COL2_PIN) == 0)
        key = 10;
    else if (gpio_get_level(COL3_PIN) == 0)
        key = 11;
    else if (gpio_get_level(COL4_PIN) == 0)
        key = 12;
    gpio_set_level(ROW3_PIN, 1);

    // Set Row 4 Low and check columns
    gpio_set_level(ROW4_PIN, 0);
    if (gpio_get_level(COL1_PIN) == 0)
        key = 13;
    else if (gpio_get_level(COL2_PIN) == 0)
        key = 14;
    else if (gpio_get_level(COL3_PIN) == 0)
        key = 15;
    else if (gpio_get_level(COL4_PIN) == 0)
        key = 16;
    gpio_set_level(ROW4_PIN, 1);

    return key;
}