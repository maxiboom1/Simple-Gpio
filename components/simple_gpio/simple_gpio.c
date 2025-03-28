#include "simple_gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define TAG "SIMPLE_GPIO"
#define QUEUE_SIZE 10  // Queue size for holding events

// Global variables to store configuration
static const gpio_num_t* g_pins = NULL;       // Array of pins to monitor
static uint8_t g_pin_count = 0;               // Number of pins
static gpio_event_handler_t g_handler = NULL; // User’s callback function
static QueueHandle_t gpio_evt_queue = NULL;   // Queue for ISR-to-task communication

// Event struct to pass pin and level
typedef struct {
    gpio_num_t pin;
    int level;
} gpio_event_t;

// ISR handler: runs when a pin changes state
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    gpio_num_t pin = (gpio_num_t)(int)arg;  // Pin number passed as arg
    int level = gpio_get_level(pin);        // Read current state
    gpio_event_t evt = { pin, level };      // Bundle event data
    xQueueSendFromISR(gpio_evt_queue, &evt, NULL);  // Queue it fast
}

// Task: processes queued events and calls the user’s handler
static void gpio_task(void* arg) {
    gpio_event_t evt;
    while (1) {
        if (xQueueReceive(gpio_evt_queue, &evt, portMAX_DELAY)) {
            if (g_handler) {
                g_handler(evt.pin, evt.level);  // Call user’s callback with pin and level
            }
        }
    }
}

esp_err_t simple_gpio_init(const gpio_num_t* pins, uint8_t pin_count, gpio_event_handler_t handler) {
    if (pins == NULL || pin_count == 0 || handler == NULL) {
        return ESP_ERR_INVALID_ARG;  // Validate inputs
    }

    g_pins = pins;          // Store pin array
    g_pin_count = pin_count;// Store pin count
    g_handler = handler;    // Store callback

    // Create queue for events
    gpio_evt_queue = xQueueCreate(QUEUE_SIZE, sizeof(gpio_event_t));
    if (gpio_evt_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create queue");
        return ESP_FAIL;
    }

    // Enable ISR service (needed once for all GPIOs)
    gpio_install_isr_service(0);

    // Configure each pin and attach ISR
    for (uint8_t i = 0; i < pin_count; i++) {
        gpio_config_t io_conf = {
            .pin_bit_mask = 1ULL << g_pins[i],
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_ANYEDGE  // Trigger on any edge (HIGH-to-LOW or LOW-to-HIGH)
        };
        gpio_config(&io_conf);
        gpio_isr_handler_add(g_pins[i], gpio_isr_handler, (void*)(int)g_pins[i]);
    }

    ESP_LOGI(TAG, "GPIO library initialized with %d pins", pin_count);
    return ESP_OK;
}

esp_err_t simple_gpio_start(void) {
    if (g_pins == NULL || g_handler == NULL || gpio_evt_queue == NULL) {
        return ESP_ERR_INVALID_STATE;  // Ensure init was called properly
    }
    xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);  // Start the task
    ESP_LOGI(TAG, "GPIO monitoring started");
    return ESP_OK;
}