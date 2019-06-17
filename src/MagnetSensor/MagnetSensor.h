/**
 * @file MagnetSensor.h
 * @author Dominic Möri (mdomin470@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-06-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
// Dieses File enthält Reccourcen für den Magentsensor xy
#include <driver/gpio.h>
#include <esp_log.h>
#include <iostream>
#include "i2c/I2C.h"

#define MAG_TAG "Magnet_Sensor"

namespace magnetsens {
    static uint8_t angle;
    extern uint8_t buffer[3];
    extern uint8_t raw[10];

    void init();
    // Calibrate Magnet sensor
    void calibrate();
    // Gibt den Rohen Sensorwert zurück
    float getRaw();
    // Gibt den Aktuellen Winkel in Grad zurück
    float getAngle();

}  // namespace magnetsens