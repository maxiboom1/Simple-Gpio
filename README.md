# Simple GPIO Library for ESP-IDF

A lightweight library to simplify GPIO event handling in ESP-IDF, abstracting away the low-level setup so developers can focus on handling pin events.

## Background: GPIO Setup in ESP-IDF

To work with GPIO pins in ESP-IDF, developers typically need to set up the following infrastructure:

- **Init Pins:** Configure GPIO pins (e.g., input mode, pull-ups, interrupt type).
- **Start ISR:** Install the ISR service (`gpio_install_isr_service`).
- **Register ISR Handler:** Attach an ISR handler to each pin, running in IRAM for speed (`gpio_isr_handler_add`).
- **Create Queue:** Set up a FreeRTOS queue (`xQueueCreate`) to pass events from ISR to task.
- **Create Task:** Spawn a FreeRTOS task (`xTaskCreate`) to process events.

**Flow:**
1. The ISR handler catches pin state changes (e.g., HIGH to LOW) and quickly pushes them to the queue.
2. The task, running in a loop, receives events via `xQueueReceive` and implements the handling logic.

This setup is repetitive and complex for simple use cases where you just want to react to pin changes.

## How Simple GPIO Solves This

The `simple_gpio` library hides all this complexity. You only need to:
1. Declare the pins you want to monitor.
2. Provide a callback function to handle events.

The library internally manages the ISR, queue, and task, delivering pin events directly to your callback.

### Features
- Monitor multiple GPIO pins with one handler.
- No manual ISR, queue, or task management.
- Simple API: initialize and start.

### Usage Example

```c
#include "simple_gpio.h"
#include "esp_log.h"

#define TAG "APP"

// Callback to handle pin events
void handle_pin_event(gpio_num_t pin, int level) {
    ESP_LOGI(TAG, "Pin %d changed to %d", pin, level);
}

void app_main(void) {
    gpio_num_t pins[] = { GPIO_NUM_32, GPIO_NUM_33 }; // Pins to monitor
    simple_gpio_init(pins, 2, handle_pin_event);      // Init with pins and handler
    simple_gpio_start();                              // Start monitoring
}
```

### Notes

- This is not tested yet - its just an idea and mockup code. Anyone who interested in such tool please write me to galutziogen@gmail.com - we can polish this together - I"ll be more than happy :)