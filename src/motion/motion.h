#include <iostream>
#include "esp_err.h"

namespace motion
{
    esp_err_t initServo();

    esp_err_t initStepperMotor();

    esp_err_t setYaxis(float angle, uint8_t velocity = 0, uint8_t acceleration = 0);

    esp_err_t setXaxis(float angle, uint8_t velocity = 0, uint8_t acceleration = 0);

    esp_err_t deinitServo();

    esp_err_t deinitStepperMotor();
}