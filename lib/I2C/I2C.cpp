/**
 * @file I2C.cpp
 * @author Dominic Möri (mdomin470@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <I2C.h>

namespace i2c
{
    
i2c_port_t I2CPort;
bool check = false;

/**
 * @brief 
 * 
 * @param Port 
 * @param Mode 
 * @param Frequency 
 * @param Sda 
 * @param Scl 
 * @return uint8_t 
 */
uint8_t init(i2c_port_t Port, i2c_mode_t Mode, uint32_t Frequency, gpio_num_t Sda, gpio_num_t Scl)
{
    I2CPort = Port;
    if (check == true)
    {
        ESP_LOGI(ERROR_TAG, "The Function i2c::init was already initalised!");
        return -1;
    }

    ESP_LOGI(I2C_TAG, "Initialise I2C...");
    //Konfigurationsparameter setzen
    i2c_config_t conf = {
        .mode = Mode,
        .sda_io_num = Sda,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = Scl,
        .scl_pullup_en = GPIO_PULLUP_ENABLE
    };

    if (conf.mode == I2C_MODE_MASTER)
    {
        conf.master.clk_speed = Frequency;
    }
    //Wenn I2C als Slave benutzt wird
    else if (conf.mode == I2C_MODE_SLAVE)
    {
        //Keine 10Bit Addresse
        conf.slave.addr_10bit_en = 0;
        //Geräteaddresse vom Slave ist 0x42
        conf.slave.slave_addr = 0x42;
    }

    //Konfiguriere den I2C Treiber
    if (i2c_param_config(Port, &conf) != ESP_OK)
    {
        ESP_LOGI(ERROR_TAG, "I2C.cpp Error: I2C driver configuration was not successful");
        return -1;
    }

    //installiere Treiber ohne Buffer und ohne Flags, die gesetzt werden
    if (i2c_driver_install(Port, Mode, 0, 0, 0) != ESP_OK)
    {
        ESP_LOGI(ERROR_TAG, "I2C.cpp Error: I2C driver installation was not successful");
        return -1;
    }
    ESP_LOGI(I2C_TAG, "I2C Successfully initialised");

    check = true;
    return 0;
}

//Lese I2C Daten: Der Pointer zu Data sind die empfangenen Daten
//der Data Pointer sollte auf ein Array zeigen, welches mindestend so gross wie die erwarteten Datenpakete ist
/**
 * @brief 
 * 
 * @param SlaveAddress 
 * @param n 
 * @param Data 
 */
void read(uint8_t SlaveAddress, int n, uint8_t *Data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, SlaveAddress, ACK);
    i2c_master_read(cmd, Data, n, I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2CPort, cmd, 50 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
}

//Rückgabewert: True wenn Acknowledged vom Slave, False wenn die Übertragung scheiterte
/**
 * @brief 
 * 
 * @param SlaveAddress 
 * @param n 
 * @param Data 
 */
void write(uint8_t SlaveAddress,uint8_t n, uint8_t *Data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, SlaveAddress, ACK);
    i2c_master_write(cmd, Data, n, ACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2CPort, cmd, 50 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
}

} // namespace i2c