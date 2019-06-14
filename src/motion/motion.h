#include "esp_err.h"
#include "SPI/SPI.h"
#include <iostream>

namespace motion {
  
    namespace servo {
        esp_err_t init();

        extern float angle;
        esp_err_t setAxis(float angle, uint8_t velocity = 0, uint8_t acceleration = 0);

        esp_err_t deinit();
    } // namespace servo

    namespace stepper {

        esp_err_t init();

        esp_err_t setAxis(float angle, uint8_t velocity = 0, uint8_t acceleration = 0);

        esp_err_t deinit();

    } // namespace stepper

} // namespace motion