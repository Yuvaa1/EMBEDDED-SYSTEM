#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#define RXD_PIN (GPIO_NUM_16)  // UART RX pin
#define BAUD_RATE 115200       // UART baud rate

void uart_receiver_task(void *pvParameters) {
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, UART_PIN_NO_CHANGE, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, 256, 0, 0, NULL, 0);

    uint8_t data;
    while (1) {
        int len = uart_read_bytes(UART_NUM_1, &data, 1, portMAX_DELAY);
        if (len > 0) {
            printf("Received data: %c\n", data);
        }
    }
}

void app_main() {
    xTaskCreate(uart_receiver_task, "uart_receiver_task", 2048, NULL, 5, NULL);
}
