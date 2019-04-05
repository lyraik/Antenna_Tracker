#include <MagnetSensor.h>


/*

MagnetSensor::MagnetSensor()
{   
    //Code zum Initialisieren des Sensors
    ESP_LOGI(MAG_TAG, "Initialising Magnet Sensor...");
    i2c_port_t Port = I2C_NUM_0;
    gpio_num_t sda = GPIO_NUM_21;
    gpio_num_t scl = GPIO_NUM_22;
    MagSens = new I2C(Port, I2C_MODE_MASTER, 100000,sda,scl);
    MagSens->init();
    ESP_LOGI(MAG_TAG, "Magnet Sensor initalised");
    uint8_t angle[2] = {0,0};
}

void MagnetSensor::calibrate()
{
    ESP_LOGI(MAG_TAG,"Calibrating Magnetsensor...");
    //Kalibriere den Magnetsensor
}

float MagnetSensor::GetAngle()
{
    float angle = 0;
    //Code zum lesen des Magnetsensor Werts

    return angle;
}

float MagnetSensor::GetRaw()
{
    uint8_t raw = 0;

    //Code zum lesen des Rohen Sensorwerts
 
    //0x3D: Sensor read addresse
    //ox3C: Sensor write addresse
    MagSens->write(0x3C,0x28);
    MagSens->read(0x3D,1,&raw);
 
 
    return raw;
}
*/

void InitMagnetSensor()
{
    ESP_LOGI(MAG_TAG, "Initialising Magnet Sensor...");
    i2c_port_t Port = I2C_NUM_0;
    gpio_num_t sda = GPIO_NUM_21;
    gpio_num_t scl = GPIO_NUM_22;
    I2CInit(Port, I2C_MODE_MASTER, 100000,sda,scl);
   // MagSens->init();
    ESP_LOGI(MAG_TAG, "Magnet Sensor initalised");
    uint8_t angle[2] = {0,0};
}
//Calibrate Magnet sensor
void CalibrateMagnetsensor()
{
    ESP_LOGI(MAG_TAG, "calibrating...");

}
//Gibt den Rohen Sensorwert zurück
float GetRaw()
{
    uint8_t buffer[2];

    writeI2C(0x3C,0x28);
    readI2C(0x3D,1,buffer);
    return static_cast<float>(buffer[0]);
}
//Gibt den Aktuellen Winkel in Grad zurück
float GetAngle()
{
    return 2.71;
}

