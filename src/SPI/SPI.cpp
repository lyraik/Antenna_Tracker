#include "SPI.h"

namespace SPI {

    spi_device_handle_t spi = NULL;

    spi_bus_config_t buscfg;

    spi_device_interface_config_t devcfg;


    esp_err_t init(uint8_t miso, uint8_t mosi, uint8_t clk, uint8_t ss) {

           
            buscfg.miso_io_num = miso;
            buscfg.mosi_io_num = mosi;
            buscfg.sclk_io_num = clk;
            buscfg.quadwp_io_num = -1;
            buscfg.quadhd_io_num = -1;
            buscfg.max_transfer_sz = 32 * 8; // in bits (this is for 32 bytes. adjust as needed)
        

            devcfg.clock_speed_hz = 1000000; // this is for 1MHz. adjust as needed
            devcfg.mode = 0;
            devcfg.spics_io_num = ss;
            devcfg.queue_size = 3; // how many transactions will be queued at once
        

        if (spi_bus_initialize(HSPI_HOST, &buscfg, 1) != ESP_OK) {
            return ESP_FAIL;
        }

        if (spi_bus_add_device(HSPI_HOST, &devcfg, &spi) != ESP_OK) {
            return ESP_FAIL;
        }

        return ESP_OK;
    }

    esp_err_t write(uint8_t addr, uint8_t length, uint8_t *buffer) {
        spi_transaction_t comm;

        comm.addr = addr, comm.tx_buffer = static_cast<const void *>(buffer);

        spi_device_transmit(spi, &comm);

        return ESP_OK;
    }

    esp_err_t deinit() {

        return ESP_OK;
    }

} // namespace SPI
