/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_int_wdt.h"
#include "esp_log.h"
#include "esp_spp_api.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "soc/rtc_wdt.h"

#include "math.h"

// Enthält Parameter welche den Zustand des Antenna Trackers und der Drohne
// beschreiben #include "config.h"

// Enthält Bluetooth Reccourcen & Bluetooth Task
#include "Bluetooth/Bluetooth.h"

// Enthält GPS Reccourcen
#include "GPS/GPS.h"

// Entwält I2C Reccourcen
#include "MagnetSensor/MagnetSensor.h"

// Server Reccourcen
#include "Webserver/Webserver.h"

// Motor Reccourcen
#include "Motion/Motion.h"

#include "Webserver/Webserver.h"
#include "Webserver/Wifi.h"
#include "Filesystem/Filesystem.h"

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
void orientationTask(void* params) {
    uint16_t north = 0;
    uint16_t angle = 0;
    float magbuff[360];

    /**
     *
     * @brief Construct a new magnetsens::Init object
     *
     */
    magnetsens::init();

    // Gehe zu Stopp-position Code dafür muss noch geschrieben werden.
    // Hardwaretreiber noch nicht geschtieben!

    // Mache eine Runde mit dem Antenna-Tracker und suche den Wert vom Magnet
    // sensor heraus, der zu Norden passt North wird zu diesem Wert (Referenz für
    // alle späteren Berechnungen)
    ESP_LOGI(POS_TAG, "triangulating position...");

    magnetsens::calibrate();

    // Fahre einmal herum mit dem Antenna Tracker, lese in jedem Winkel das Magnetfeld ein
    // und finde den Winkel mit dem grössten Wert
    for (int i = 0; i < 360; i++) {
        motion::stepper::setAxis(i);
        magbuff[i] = magnetsens::getRaw();

        if (magbuff[i] >= magbuff[north]) {
            north = i;
        }
    }

    while (true) {
        if ((longitude - GPS::getLong()) > 0) {
            if ((lattitude - GPS::getLat()) > 0) {
                angle = 90 - atan((lattitude - GPS::getLat()) / (longitude - GPS::getLong()));
            } else {
                angle = 90 + atan(-((lattitude - GPS::getLat()) / (longitude - GPS::getLong())));
            }
        } else if ((longitude - GPS::getLong()) < 0) {
            if ((lattitude - GPS::getLat()) > 0) {
                angle = 270 + atan((lattitude - GPS::getLat()) / (longitude - GPS::getLong()));
            } else {
                angle = 270 - atan(-((lattitude - GPS::getLat()) / (longitude - GPS::getLong())));
            }
        }

        motion::stepper::setAxis(angle);

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
/**
 * @brief
 *
 * @param pvParameter
 */
void blinkTask(void* pvParameter) {
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

    while (1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void communicationTask(void* pvParameter) {
    enableBluetooth();

    if (wifi::init("Antenna Tracker", "12345678") == ESP_OK) {
        ESP_LOGI(MAIN_TAG, "\n \n started wifi successfully \n \n");
    } else {
        ESP_LOGI(MAIN_TAG, "\n \n start wifi failed \n \n");
    }
    web::init();

    while (true) {
        // remove when actually doing something
        vTaskSuspend(NULL);
    }
}
/**
 * @brief
 *
 * @param paramenter
 */
void batteryMonitoringTask(void* paramenter) {
    /**
     * @brief
     * Initialisiere I2C für ADC's, falls noch nicht initialisiert
     *
     */

    if (i2c::check == false) {
        i2c_port_t Port = I2C_NUM_0;
        gpio_num_t sda = GPIO_NUM_21;
        gpio_num_t scl = GPIO_NUM_22;
        i2c::init(Port, I2C_MODE_MASTER, 100000, sda, scl);
    }
    ESP_LOGI(MAIN_TAG, "Connecting to ADCs...");

    while (true) {
        // remove when actually doing something
        vTaskSuspend(NULL);
    }
}

extern "C" void app_main() {
    /**
     * @brief Construct a new rtc wdt disable object
     * Sehr wichtig, ansonsten setzt sich der MCU nach 20 sek zurück!
     *
     */
    rtc_wdt_disable();

    // initialize non volatile memory
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    fs::init();

    xTaskCreate(&blinkTask, "Blink", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    xTaskCreate(&orientationTask, "Orientation", 2000, NULL, 4, NULL);
    xTaskCreate(&batteryMonitoringTask, "BatterySurvailance", 3000, NULL, 3, NULL);
    xTaskCreate(&communicationTask, "Communication", 6000, NULL, 4, NULL);

    ESP_LOGI(MAIN_TAG, "Welcome to the Antenna tracker Software");
}
