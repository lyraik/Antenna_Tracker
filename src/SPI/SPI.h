/**
 * @file SPI.h
 * @author Dominic Möri (mdomin470@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-06-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <iostream>
#include "esp_err.h"
#include "driver/spi_master.h"
#include "esp_log.h"


namespace SPI
{
    extern spi_bus_config_t buscfg;

    extern spi_device_interface_config_t devcfg;

    extern spi_device_handle_t spi;
   
    extern uint16_t *rxbuffer;


    extern spi_transaction_t comm;

    
    esp_err_t init(uint8_t miso, uint8_t mosi, uint8_t clk, uint8_t ss);

    esp_err_t write(uint8_t addr, uint8_t length, uint8_t *buffer);

    esp_err_t read(uint8_t addr, uint8_t command, uint8_t length, uint8_t* buffer);

    esp_err_t deinit();

}