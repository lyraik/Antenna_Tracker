#include <I2C.h>

/*
I2C::I2C(i2c_port_t Port, i2c_mode_t Mode, uint32_t Frequency, gpio_num_t Sda, gpio_num_t Scl)
{
    //Attribute werden Parameter
    I2CPort = Port;
    I2CMode = Mode;
    I2CFrequency = Frequency;
    SdaPin = Sda;
    SclPin = Scl;
};

//Initialisiere I2C
bool I2C::init()
{
    ESP_LOGI(I2C_TAG,"Initialise I2C...");
    //Konfigurationsparameter setzen
    conf = {
        .mode = I2CMode,
        .sda_io_num = SdaPin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = SclPin,
        .scl_pullup_en = GPIO_PULLUP_ENABLE
    };

    if(conf.mode == I2C_MODE_MASTER)
    {
        conf.master.clk_speed = I2CFrequency;
    }
    //Wenn I2C als Slave benutzt wird
    else if(conf.mode == I2C_MODE_SLAVE)
    {
        //Keine 10Bit Addresse
        conf.slave.addr_10bit_en = 0;
        //Geräteaddresse vom Slave ist 0x42
        conf.slave.slave_addr = 0x42;
    }

    //Konfiguriere den I2C Treiber
    if(i2c_param_config(I2CPort,&conf) != ESP_OK)
    {
        ESP_LOGI(ERROR_TAG,"I2C.cpp Error: I2C driver configuration was not successful");
        return false;
    }

    //installiere Treiber ohne Buffer und ohne Flags, die gesetzt werden
    if(i2c_driver_install(I2CPort,I2CMode,0, 0, 0) != ESP_OK)
    {
        ESP_LOGI(ERROR_TAG,"I2C.cpp Error: I2C driver installation was not successful");
        return false;
    }
    ESP_LOGI(I2C_TAG,"I2C Successfully initialised");
    return true;
}

        
//Lese I2C Daten
void I2C::read(uint8_t SlaveAddress, int n, uint8_t *Data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, SlaveAddress<<1, ACK);
    i2c_master_read(cmd, Data, n-1,I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2CPort, cmd, 50 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
}
    
//Schreibe I2C Daten
void I2C::write(uint8_t SlaveAddress, uint8_t Data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SlaveAddress << 1) | WRITE_BIT, ACK);
    i2c_master_write(cmd, &Data, 1, ACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2CPort, cmd, 50 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
} 
*/
uint8_t I2CInit(i2c_port_t Port, i2c_mode_t Mode, uint32_t Frequency, gpio_num_t Sda, gpio_num_t Scl)
{
    //Attribute werden Parameter
    I2CPort = Port;
    I2CMode = Mode;
    I2CFrequency = Frequency;
    SdaPin = Sda;
    SclPin = Scl;

    ESP_LOGI(I2C_TAG,"Initialise I2C...");
    //Konfigurationsparameter setzen
    conf = {
        .mode = I2CMode,
        .sda_io_num = SdaPin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = SclPin,
        .scl_pullup_en = GPIO_PULLUP_ENABLE
    };

    if(conf.mode == I2C_MODE_MASTER)
    {
        conf.master.clk_speed = I2CFrequency;
    }
    //Wenn I2C als Slave benutzt wird
    else if(conf.mode == I2C_MODE_SLAVE)
    {
        //Keine 10Bit Addresse
        conf.slave.addr_10bit_en = 0;
        //Geräteaddresse vom Slave ist 0x42
        conf.slave.slave_addr = 0x42;
    }

    //Konfiguriere den I2C Treiber
    if(i2c_param_config(I2CPort,&conf) != ESP_OK)
    {
        ESP_LOGI(ERROR_TAG,"I2C.cpp Error: I2C driver configuration was not successful");
        return false;
    }

    //installiere Treiber ohne Buffer und ohne Flags, die gesetzt werden
    if(i2c_driver_install(I2CPort,I2CMode,0, 0, 0) != ESP_OK)
    {
        ESP_LOGI(ERROR_TAG,"I2C.cpp Error: I2C driver installation was not successful");
        return false;
    }
    ESP_LOGI(I2C_TAG,"I2C Successfully initialised");
    return true;
}
        
//Initialisiere I2c mit den Parametern aus den Konstruktor
bool init()
{

    return false;    
}

//Lese I2C Daten: Der Pointer zu Data sind die empfangenen Daten
//der Data Pointer sollte auf ein Array zeigen, welches mindestend so gross wie die erwarteten Datenpakete ist 
void readI2C(uint8_t SlaveAddress, int n, uint8_t *Data)
{
     i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, SlaveAddress<<1, ACK);
    i2c_master_read(cmd, Data, n-1,I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2CPort, cmd, 50 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
}

//Rückgabewert: True wenn Acknowledged vom Slave, False wenn die Übertragung scheiterte
void writeI2C(uint8_t SlaveAddress, uint8_t Data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SlaveAddress << 1) | WRITE_BIT, ACK);
    i2c_master_write(cmd, &Data, 1, ACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2CPort, cmd, 50 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
}