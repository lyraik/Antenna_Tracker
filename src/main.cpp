/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <iostream>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "driver/gpio.h"

//Enthält Parameter welche den Zustand des Antenna Trackers und der Drohne beschreiben
//#include "config.h"

//Enthält Bluetooth Reccourcen & Bluetooth Task
#include "Bluetooth.h"

//Enthält GPS Reccourcen
#include "GPS.h"

//Entwält I2C Reccourcen
#include "MagnetSensor.h"

#define MAG_VAL_TAG "Magnetic Sensor value"
#define MAIN_TAG "Antenna Tracker"
#define POS_TAG "Positioningstask"

/**
 * @brief 
 * 
 * @param params
 * 
 * WARNING!! Diesr Task muss genügend Stack erhalten wenn er erstellt wird! 
 */
void positioningsTask(void *params)
{
    /**
     * @brief Construct a new magnetsens::Init object
     * 
     */
    magnetsens::Init();

    //Gehe zu Stopp-position Code dafür muss noch geschrieben werden.
    //Hardwaretreiber noch nicht geschtieben!

    //Mache eine Runde mit dem Antenna-Tracker und suche den Wert vom Magnet sensor heraus,
    //der zu Norden passt
    //North wird zu diesem Wert (Referenz für alle späteren Berechnungen)
    ESP_LOGI(POS_TAG, "triangulating position...");
    
    magnetsens::Calibrate();
    while (1)
    {
        ESP_LOGI(POS_TAG, " value: \t %f" ,magnetsens::GetRaw());
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
/**
 * @brief 
 * 
 * @param pvParameter 
 */
void blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_num_t BLINK_GPIO = GPIO_NUM_2;

    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    while (1)
    {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief 
 * 
 * @param paramenter 
 */
void BatterySurvailance(void *paramenter)
{

    /**
     * @brief 
     * Initialisiere I2C für ADC's, falls noch nicht initialisiert
     * 
     */
    if(i2c::check != true)
    {
        i2c_port_t Port = I2C_NUM_0;
        gpio_num_t sda = GPIO_NUM_21;
        gpio_num_t scl = GPIO_NUM_22;
        i2c::init(Port, I2C_MODE_MASTER, 100000, sda, scl);
    }
    ESP_LOGI(I2C_TAG,"Connecting to ADCs...");


    while(1)
    {




    }
}

extern "C"
{
    void app_main()
    {
        enableBluetooth();
        // xTaskCreate(&bluetooth_task,"Bluetooth_task",configMINIMAL_STACK_SIZE,NULL,5,NULL);
        xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
        xTaskCreate(&positioningsTask, "postask", 2000, NULL, 4, NULL);
        xTaskCreate(&BatterySurvailance,"BatterySurvailance",2000,NULL,4,NULL);

        ESP_LOGI(MAIN_TAG, "Welcome to the Antenna tracker Software");
        vTaskDelay(100 / portTICK_PERIOD_MS);

        while (1)
        {
            /* Blink on (output high) */
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
}
