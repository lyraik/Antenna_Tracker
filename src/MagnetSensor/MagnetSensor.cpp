/**
 * @file MagnetSensor.cpp
 *
 * @author Dominic Möri (mdomin470@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-04-07
 *
 * @copyright Copyright (c) 2019
 *
 */
#include "MagnetSensor.h"

namespace magnetsens {

    /**
     * @brief
     *
     */
    void init() {
        ESP_LOGI(MAG_TAG, "Initialising Magnet Sensor...");
        i2c_port_t Port = I2C_NUM_0;
        gpio_num_t sda = GPIO_NUM_21;
        gpio_num_t scl = GPIO_NUM_22;

        if (!i2c::check) {
            if (i2c::init(Port, I2C_MODE_MASTER, 100000, sda, scl) == -1) {
                ESP_LOGI(ERROR_TAG, "Failed to Init magnetsensor!, I2C already Initalised");
                return;
            }
            ESP_LOGI(MAG_TAG, "Magnet Sensor initalised");

            uint8_t buffer[3] = {0x22, 0x00, 0x00};
            i2c::write(0x3C, 2, buffer);
        }
    }

    /**
     * @brief
     *  Calibrate Magnet sensor
     */
    void calibrate() {
        ESP_LOGI(MAG_TAG, "calibrating...");
    }

    /**
     * @brief Get the Raw object
     * Gibt den Rohen Sensorwert zurück
     *
     * @return float
     *
     */
    float getRaw() {
        float val;
        uint8_t raw[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t MagReg = 0x28;
        i2c::write(0x3C, 1, &MagReg + 1);
        i2c::read(0x3D, 1, raw);
        val = static_cast<float>(raw[0]);

        return val;
    }
    /**
     * @brief Get the Angle object
     * Gibt den Aktuellen Winkel in Grad zurück
     *
     * @return float
     */
    float getAngle() {
        // Nicht ein wirklicher Rückgabewert!!!!!!!
        // nur da, weil der eigentliche Code noch nicht geschrieben wurde!!!!!!
        return 2.71;
    }
}  // namespace magnetsens