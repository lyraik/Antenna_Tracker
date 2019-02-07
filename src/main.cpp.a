/**
 * @brief Blink example, toggles GPIO33 with 2Hz
 * 
 * Code from this example is from https://github.com/espressif/esp-idf/blob/master/examples/get-started/blink/main/blink.c
 * 
 * @file main.cpp
 * @author Dominik Gschwind
 * @date 2018/09/09
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio.h"
#include "sdkconfig.h"

void blink_task(void*){
    // configures pin as GPIO
    // could be configured something else at startup
    gpio_pad_select_gpio(GPIO_NUM_33);
    // set direction to output
    gpio_set_direction(GPIO_NUM_33, GPIO_MODE_OUTPUT);

    while(1){
        // set gpio low
        gpio_set_level(GPIO_NUM_33, 0);
        // delay task for 500ms
        vTaskDelay(500 / portTICK_PERIOD_MS);
        // set gpio high
        gpio_set_level(GPIO_NUM_33, 1);
        // delay task for 500ms
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Gets called after start
 * 
 * Must be extern "C", because sdk expects C linkage
 */
extern "C" void app_main(){
    xTaskCreate(&blink_task, "gpio_on_off", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}