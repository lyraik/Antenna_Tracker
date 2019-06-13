//I2C Wandler
/*

#include <stdio.h>
#include <i2c.h>
#include <stdio.h>
#include "driver/i2c.h"




#include "esp_types.h"
#include "esp_event.h"
#include "esp_err.h"
#include "driver/uart.h"


void driver_i2c_startup();

uint8_t driver_i2c_transmit();

uint8_t driver_i2c_receive();


#define I2C_ACK 0x00;
#define I2C_NACK 0x01;



void driver_i2c_stratup()
{
    uint8_t i2c_sda_pin_num = 12;
    uint8_t i2c_scl_pin_num = 13;
    uint32_t i2c_clk_frqu = 0xA0;



    i2c_config_t startup = {
      startup.mode = I2C_MODE_MASTER,
      
      startup.sda_io_num = i2c_sda_pin_num,
      startup.sda_pullup_en = GPIO_PULLUP_ENABLE,
                      
      startup.scl_io_num = i2c_scl_pin_num,
      startup.scl_pullup_en = GPIO_PULLUP_ENABLE,

      startup.master.clk_speed = i2c_clk_frqu,
      
      //not needed:
      //startup.addr_10bit_en
      //startup.slave_addr

      //startup.i2c_opmode_t, //Dominik/c fragen
      //esp_err_t i2c_param_config(I2C_NUM_1,&startup)


    };
    i2c_mode_t  

    i2c_driver_install(I2C_NUM_1,I2C_MODE_MASTER,&startup,0);
    i2c_param_config(I2C_NUM_1, &startup);
    

}

uint8_t driver_i2c_transmit(uint8_t *data,uint8_t Address,uint8_t Lenght)
{
  data = malloc (Lenght);
  uint8_t Data_zerstückelt;
  uint tmp_Data;
  char shiftWert;
  float i2c_SchleifenZaehler = 0;
  i2c_cmd_handle_t cmd;
  i2c_port_t I2C_Port = 0x01;
  cmd = i2c_cmd_link_create();
  i2c_master_start (cmd);
  i2c_master_write_byte(cmd,  (Address << 1) | I2C_MASTER_WRITE,0x01); //Adresse übertragen
  for(i2c_SchleifenZaehler =1;  i2c_SchleifenZaehler>=Lenght;  i2c_SchleifenZaehler++  )
  {
    shiftWert = (i2c_SchleifenZaehler * 8);
    tmp_Data = (*data >> shiftWert );
    i2c_master_write(cmd,tmp_Data,1,0x01);
  }
  i2c_master_cmd_begin(I2C_Port,cmd,1000);
  i2c_master_stop(cmd);
  i2c_cmd_link_delete(cmd);
  i2c_SchleifenZaehler = 0;
}

uint8_t driver_i2c_transmitV2(uint8_t *data,uint8_t Address,uint8_t MemoryAddress ,uint8_t Lenght) // Autoincrease
{
  data = malloc (Lenght);
  uint8_t Data_zerstückelt;
  uint tmp_Data;
  char shiftWert;
  float i2c_SchleifenZaehler = 0;
  i2c_cmd_handle_t cmd;
  cmd = i2c_cmd_link_create();
  i2c_master_start (cmd);
  i2c_master_write_byte(cmd,  (Address << 1) | I2C_MASTER_WRITE,0x01); //Adresse übertragen
  i2c_master_write_byte(cmd, MemoryAddress, 0x01);
  i2c_master_write(cmd,Address,data,0x01);
  i2c_master_write_byte(cmd, data ,0x01);
  
  i2c_master_stop(cmd);
  i2c_cmd_link_delete(cmd);
  i2c_SchleifenZaehler = 0;
}
uint driver_i2c_create()
{
  i2c_cmd_handle_t i2c_consolen_variabel_tmp;
  i2c_consolen_variabel_tmp = i2c_cmd_link_create();
  i2c_master_start (i2c_consolen_variabel_tmp);
  return i2c_consolen_variabel_tmp;
}

uint driver_i2c_write_deklarieren(i2c_cmd_handle_t *cmd,uint8_t *data,uint8_t Address,uint8_t MemoryAddress)
{
  i2c_master_write_byte(cmd,  (Address << 1) | I2C_MASTER_WRITE,0x01); //Adresse übertragen
  i2c_master_write_byte(cmd, MemoryAddress, 0x01);
  i2c_master_write_byte(cmd, data, 0x01);
}

uint driver_i2c_send_cmd(i2c_cmd_handle_t *cmd, i2c_port_t Port)
{
  return i2c_master_cmd_begin (Port,cmd,1000);
}

uint8_t driver_i2c_receive()
{
  i2c_cmd_handle_t cmd;
  cmd = i2c_cmd_link_create();
  i2c_master_start (cmd);
  i2c_master_read_byte(cmd,   (Address << 1) | I2C_MASTER_READ,0x01);
  i2c_master_read(cmd,&data,Lenght,0x01);
  i2c_master_stop(cmd);
  i2c_cmd_link_delete(cmd);
}

*/