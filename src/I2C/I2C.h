/**
 * @file I2C.h
 *
 * @author Dominic Möri (mdomin470@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-04-07
 *
 * @copyright Copyright (c) 2019
 *
 */
#include <driver/i2c.h>
#include <stdio.h>
#include "esp_log.h"

/**************************Tags**************************/
#define ERROR_TAG "ESP_ERROR"
#define I2C_TAG "ESP_I2C"

/**********Namespace i2c contains all i2c reccources**********/
namespace i2c {
    constexpr uint8_t ACK = 0x01;
    constexpr uint8_t WRITE_BIT = 0x01;
    constexpr uint8_t READ_BIR = 0x0;

    // Konfigurationsparameter
    extern i2c_config_t conf;
    /**
     * @brief
     *  I2C Port
     */
    extern i2c_port_t I2CPort;

    extern bool check;
    /**
     * @brief
     *  Check wird beim ersten initalisieren gesetzt und beim
     *  deinitialisieren gelöscht, dass nur ein I2C initialisiert werden kann
     *
     */

    /**
     * @brief Initialisiert das I2C
     *
     * @param Port
     * @param Mode
     * @param Frequency
     * @param Sda
     * @param Scl
     * @return uint8_t
     *
     * Beispielparameter sind:
     *
     * Port = I2C_NUM_0 (gibt nur Port 0 und Port 1)
     * Mode = I2C_MODE_MASTER
     * Frequency: 100000
     * Sda = 21 (Der SDA Pin auf dem Pinout)
     * Scl = 22 (Der SCL Pin auf dem Pinout)
     *
     */
    uint8_t init(i2c_port_t Port, i2c_mode_t Mode, uint32_t Frequency, gpio_num_t Sda, gpio_num_t Scl);

    /**
     * @brief deinitialisiert I2C
     * @return uint8_t
     */
    uint8_t deinit(void);

    /**
     * @brief
     *
     * Lese I2C Daten: Der Pointer zu Data sind die empfangenen Daten
     * der Data Pointer sollte auf ein Array zeigen, welches mindestend
     *  so gross wie die erwarteten Datenpakete ist
     *
     * @param SlaveAddress
     * @param n
     * @param Data
     */
    void read(uint8_t SlaveAddress, int n, uint8_t* Data);

    /**
     * @brief
     * Rückgabewert: True wenn Acknowledged vom Slave, False wenn die Übertragung scheiterte
     * @param SlaveAddress
     * @param Data
     */
    void write(uint8_t SlaveAddress, uint8_t n, uint8_t* Data);

}  // namespace i2c
