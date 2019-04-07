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
#include <MagnetSensor.h>

/**
 * @brief 
 * 
 */
void InitMagnetSensor()
{
    ESP_LOGI(MAG_TAG, "Initialising Magnet Sensor...");
    i2c_port_t Port = I2C_NUM_0;
    gpio_num_t sda = GPIO_NUM_21;
    gpio_num_t scl = GPIO_NUM_22;
    i2c::init(Port, I2C_MODE_MASTER, 100000, sda, scl);
    ESP_LOGI(MAG_TAG, "Magnet Sensor initalised");
    uint8_t angle[2] = {0, 0};
}
/**
 * @brief 
 *  Calibrate Magnet sensor 
 */
void CalibrateMagnetsensor()
{
    ESP_LOGI(MAG_TAG, "calibrating...");
}

/**
 * @brief Get the Raw object
 * Gibt den Rohen Sensorwert zurück
 * 
 * @return float 
 * 
 */
float GetRaw()
{
    uint8_t buffer[2];

    i2c::write(0x3C, 0x28);
    i2c::read(0x3D, 1, buffer);
    return static_cast<float>(buffer[0]);
}
/**
 * @brief Get the Angle object
 * Gibt den Aktuellen Winkel in Grad zurück
 * 
 * @return float 
 */
float GetAngle()
{
    //Nicht ein wirklicher Rückgabewert!!!!!!!
    // nur da, weil der eigentliche Code noch nicht geschrieben wurde!!!!!!
    return 2.71;
}
