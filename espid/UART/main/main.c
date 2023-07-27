#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include <string.h>


#define TXD_PIN (GPIO_NUM_17)  // UART TX pin
#define BAUD_RATE 115200       // UART baud rate

void uart_transmitter_task(void *pvParameters) {
    uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, TXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);

    while (1) {
        const char* message = "Hello from ESP32 Board 1!\n";
        uart_write_bytes(UART_NUM_0, message, strlen(message));
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    xTaskCreate(uart_transmitter_task, "uart_transmitter_task", 2048, NULL, 5, NULL);
}
