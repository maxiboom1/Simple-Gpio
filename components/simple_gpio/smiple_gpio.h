#ifndef SIMPLE_GPIO_H
#define SIMPLE_GPIO_H

#include "driver/gpio.h"
#include "esp_err.h"

// Callback type for handling pin events (user defines this in their main code)
typedef void (*gpio_event_handler_t)(gpio_num_t pin, int level);

// Initialize GPIO pins and set the event handler
esp_err_t simple_gpio_init(const gpio_num_t* pins, uint8_t pin_count, gpio_event_handler_t handler);

// Start the GPIO monitoring task
esp_err_t simple_gpio_start(void);

#endif