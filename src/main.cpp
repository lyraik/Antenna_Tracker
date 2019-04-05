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
#include <EnvoirementRecources.h>

//Enthält Bluetooth Reccourcen & Bluetooth Task
#include <Bluetooth.h>

//Enthält GPS Reccourcen
#include <GPS.h>

//Entwält I2C Reccourcen
#include <MagnetSensor.h>

    #define MAG_VAL_TAG "Magnetic Sensor value"
    #define MAIN_TAG "Main"

void positioningsTask(void *params)
{
    float North;
    //Neues GPS Objekt erstellen (GPS wird im Konstruktor initalisiert)
    //GPS GPS1();

    //Core panikt wenn ein neues Objekt erstellt wird
   // MagnetSensor *Mysensor = new MagnetSensor();
    InitMagnetSensor();
   // Mysensor->calibrate();
    //Gehe zu Stopp-position Code dafür muss noch geschrieben werden. 
    //Hardwaretreiber noch nicht geschtieben!

    //Mache eine Runde mit dem Antenna-Tracker und suche den Wert vom Magnet sensor heraus,
    //der zu Norden passt
    //North wird zu diesem Wert (Referenz für alle späteren Berechnungen)
    
    while(1)
    {
       //ESP_LOGI(MAG_VAL_TAG, "Sensor return value: %f \n", Mysensor.GetRaw());
       vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


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

    while(1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

extern "C"
{
    void app_main()
    {    
        enableBluetooth();
       // xTaskCreate(&bluetooth_task,"Bluetooth_task",configMINIMAL_STACK_SIZE,NULL,5,NULL);
        xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
        xTaskCreate(&positioningsTask,"positioninsTask",configMINIMAL_STACK_SIZE,NULL,5,NULL);
    //  xTaskCreate(&BattSurvailanceTask,"BatterySurvailanceTask",configMINIMAL_STACK_SIZE,NULL,5,NULL);
        
        ESP_LOGI(MAIN_TAG,"Welcome to the Antenna tracker Software");
        vTaskDelay(100/portTICK_PERIOD_MS);

        while(1)
        {
            
            /* Blink on (output high) */
            vTaskDelay(100/portTICK_PERIOD_MS);
            
        }
    }
}
