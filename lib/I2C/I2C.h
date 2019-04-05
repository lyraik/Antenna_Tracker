//Dieses File enthält Reccourcen für I2C Peripherien
#include <iostream>
#include <stdio.h>
#include "esp_log.h"
#include <driver/i2c.h>
 
/**************************Defines**************************/
#define ERROR_TAG "ESP_ERROR"
#define I2C_TAG "ESP_I2C"
#define ACK         0x1
#define WRITE_BIT   0x1
#define READ_BIT    0x0

/*********************Classes*******************************/

/*
class I2C
{
    private:
        i2c_port_t I2CPort;
        i2c_mode_t I2CMode;
        uint32_t I2CFrequency;
        gpio_num_t SdaPin;
        gpio_num_t SclPin;

         //Konfigurationsparameter
        i2c_config_t conf;

    public:
        //Initialisiert I2C
        //Beispielparameter sind:
        //Port = I2C_NUM_0 (gibt nur Port 0 und Port 1)
        //Mode = I2C_MODE_MASTER
        //Frequency: 100000
        //Sda = 21 (Der SDA Pin auf dem Pinout)
        //Scl = 22 (Der SCL Pin auf dem Pinout)
        I2C(i2c_port_t Port, i2c_mode_t Mode, uint32_t Frequency, gpio_num_t Sda, gpio_num_t Scl);
        
        //Initialisiere I2c mit den Parametern aus den Konstruktor
        bool init();

        //Lese I2C Daten: Der Pointer zu Data sind die empfangenen Daten
        //der Data Pointer sollte auf ein Array zeigen, welches mindestend so gross wie die erwarteten Datenpakete ist 
        void read(uint8_t SlaveAddress, int n, uint8_t *Data);
        //Rückgabewert: True wenn Acknowledged vom Slave, False wenn die Übertragung scheiterte
        void write(uint8_t SlaveAddress, uint8_t Data);
};
*/
extern "C"
{
      i2c_port_t I2CPort;
        i2c_mode_t I2CMode;
        uint32_t I2CFrequency;
        gpio_num_t SdaPin;
        gpio_num_t SclPin;

         //Konfigurationsparameter
        i2c_config_t conf;

        //Initialisiert I2C
        //Beispielparameter sind:
        //Port = I2C_NUM_0 (gibt nur Port 0 und Port 1)
        //Mode = I2C_MODE_MASTER
        //Frequency: 100000
        //Sda = 21 (Der SDA Pin auf dem Pinout)
        //Scl = 22 (Der SCL Pin auf dem Pinout)
        uint8_t I2CInit(i2c_port_t Port, i2c_mode_t Mode, uint32_t Frequency, gpio_num_t Sda, gpio_num_t Scl);
        
        //Initialisiere I2c mit den Parametern aus den Konstruktor
        bool init();

        //Lese I2C Daten: Der Pointer zu Data sind die empfangenen Daten
        //der Data Pointer sollte auf ein Array zeigen, welches mindestend so gross wie die erwarteten Datenpakete ist 
        void readI2C(uint8_t SlaveAddress, int n, uint8_t *Data);
        //Rückgabewert: True wenn Acknowledged vom Slave, False wenn die Übertragung scheiterte
        void writeI2C(uint8_t SlaveAddress, uint8_t Data);
}
    