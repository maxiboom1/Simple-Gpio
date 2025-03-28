#include "simple_gpio.h"
#include "esp_log.h"

#define TAG "APP"

// User-defined callback to handle events
void handle_pin_event(gpio_num_t pin, int level) {
    ESP_LOGI(TAG, "Pin %d changed to %d", pin, level);
}

void app_main(void) {
    // Declare pins to monitor
    gpio_num_t pins[] = { GPIO_NUM_32, GPIO_NUM_33 };
    
    // Initialize the library with pins and handler
    simple_gpio_init(pins, 2, handle_pin_event);
    
    // Start monitoring
    simple_gpio_start();
}