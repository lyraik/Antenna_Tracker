//Dieses File enthält Reccourcen für den Magentsensor xy
#include <iostream>
#include "i2c/I2C.h"
#include <esp_log.h>
#include <driver/gpio.h>

#define MAG_TAG "Magnet_Sensor"

namespace magnetsens
{
static uint8_t angle;
extern uint8_t buffer[3];
extern uint8_t raw[10];


void Init();
//Calibrate Magnet sensor
void Calibrate();
//Gibt den Rohen Sensorwert zurück
float GetRaw();
//Gibt den Aktuellen Winkel in Grad zurück
float GetAngle();

} // namespace magnetsens