//Dieses File enthält Reccourcen für den Magentsensor xy
#include <iostream>
#include <I2C.h>
#include <esp_log.h>
#include <driver/gpio.h>

#define MAG_TAG "Magnet_Sensor"

static uint8_t angle;

void InitMagnetSensor();
//Calibrate Magnet sensor
void calibrateMagnetsensor();
//Gibt den Rohen Sensorwert zurück
float GetRawMagnetsensor();
//Gibt den Aktuellen Winkel in Grad zurück
float GetAngleMagnetsensor();
/*
class MagnetSensor
{
    private:
     I2C *MagSens;
    public:
        MagnetSensor();
        //Calibrate Magnet sensor
        void calibrate();
        //Gibt den Rohen Sensorwert zurück
        float GetRaw();
        //Gibt den Aktuellen Winkel in Grad zurück
        float GetAngle();

};
*/
