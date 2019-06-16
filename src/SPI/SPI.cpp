#include "SPI.h"

namespace SPI {

    spi_device_handle_t spi;

    spi_bus_config_t buscfg;

    spi_device_interface_config_t devcfg;

    uint16_t *rxbuffer;
    spi_transaction_t comm;


    esp_err_t init(uint8_t miso, uint8_t mosi, uint8_t clk, uint8_t ss) {

           
            buscfg.miso_io_num = miso;
            buscfg.mosi_io_num = mosi;
            buscfg.sclk_io_num = clk;
            buscfg.quadwp_io_num = -1;
            buscfg.quadhd_io_num = -1;
            buscfg.max_transfer_sz = (4 * 8); // in bits (this is for 4 bytes. adjust as needed)
        

            devcfg.clock_speed_hz = 1000000; // this is for 1MHz. adjust as needed
            devcfg.mode = 0;
            devcfg.spics_io_num = ss;
            devcfg.queue_size = 3; // how many transactions will be queued at once
        
            //uint32_t flags_o = ~(SPICOMMON_BUSFLAG_NATIVE_PINS);
           // spicommon_bus_initialize_io(HSPI_HOST,&buscfg,0,NULL,&flags_o);
       
        if (spi_bus_initialize(HSPI_HOST, &buscfg, 0) != ESP_OK) {
            return ESP_FAIL;
        }

        if (spi_bus_add_device(HSPI_HOST, &devcfg, &spi) != ESP_OK) {
            return ESP_FAIL;
        }

        comm.flags |= SPI_TRANS_USE_TXDATA;
        comm.flags |= SPI_TRANS_USE_RXDATA;

        return ESP_OK;
    }


    esp_err_t write(uint8_t addr, uint8_t length, uint8_t *buffer) {        
        comm.addr = addr;
        comm.tx_data[0] = *buffer;
        comm.tx_data[1] = *(buffer+1);        
        comm.tx_data[2] = *(buffer+2);
        comm.tx_data[3] = *(buffer+3);
        
        ESP_LOGI("SPI", "Write:\t%d\tto\t%d", *buffer, addr);

        spi_device_queue_trans(spi, &comm,10);
        return ESP_OK;
    }

    esp_err_t read(uint8_t addr, uint8_t command, uint8_t length, uint8_t* buffer)
    {
        comm.addr = addr;
        comm.tx_data[0] = command;

        ESP_LOGI("SPI", "read:\t%d\tfrom\t%d\treg\t%d", *buffer, addr, command);

        spi_device_transmit(spi,&comm);

        buffer = comm.tx_data;
        return ESP_OK;

    }

    esp_err_t deinit() {

        return ESP_OK;
    }

} // namespace SPI
