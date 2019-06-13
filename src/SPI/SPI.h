#include <iostream>
#include "esp_err.h"
#include "driver/spi_master.h"

namespace SPI
{
    extern spi_bus_config_t buscfg;

    extern spi_device_interface_config_t devcfg;

    extern spi_device_handle_t spi;
    
    esp_err_t init(uint8_t miso, uint8_t mosi, uint8_t clk, uint8_t ss);

    esp_err_t write();

    esp_err_t deinit();

}