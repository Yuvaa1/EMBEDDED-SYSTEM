#include <stdio.h>
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#define GPIO_PWM0A_OUT 15   // Set GPIO 15 as PWM0A
#define GPIO_PWM0B_OUT 16   // Set GPIO 16 as PWM0B
#define PUSH_BUTTON_PIN_SPEED 33

int buttonState = 0;
int count_value = 0;
int prestate = 0;

static void mcpwm_example_gpio_initialize(void)
{
    printf("Initializing MCPWM GPIO...\n");
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_PWM0A_OUT);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, GPIO_PWM0B_OUT);
}

static void brushed_motor_forward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, float duty_cycle)
{
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_B);
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_A, duty_cycle);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}

static void brushed_motor_backward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num, float duty_cycle)
{
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_A);
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_B, duty_cycle);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
}

static void brushed_motor_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_A);
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_B);
}

static void mcpwm_example_brushed_motor_control(void *arg)
{
    // 1. MCPWM GPIO initialization
    mcpwm_example_gpio_initialize();

    // 2. Initial MCPWM configuration
    printf("Configuring initial parameters of MCPWM...\n");
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 1000;    // Frequency = 1000Hz
    pwm_config.cmpr_a = 0;    // Duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;    // Duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    // Configure PWM0A & PWM0B with above settings

    brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, 20.0);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    brushed_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, 20.0);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    gpio_set_direction(PUSH_BUTTON_PIN_SPEED, GPIO_MODE_INPUT);

    while (1) {
        buttonState = gpio_get_level(PUSH_BUTTON_PIN_SPEED);

        if (buttonState == 1 && prestate == 0) {
            count_value += 10;
            float duty_cycle = count_value;
            brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, duty_cycle);
            prestate = 1;
        } else if (buttonState == 0) {
            prestate = 0;
            brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    printf("Testing brushed motor...\n");
    xTaskCreate(mcpwm_example_brushed_motor_control, "mcpwm_example_brushed_motor_control", 4096, NULL, 5, NULL);
}
