#include "Motion.h"
namespace motion {

    namespace servo {

        esp_err_t init() {
            
            angle = 0;
            return ESP_OK;
        }

        esp_err_t setAxis(float angle, uint8_t velocity, uint8_t acceleration) {
        
            return ESP_OK;
        }

        esp_err_t deinit() {

            return ESP_OK;
        }
    } // namespace servo

    namespace stepper {

        esp_err_t init() {
            
            
            SPI::init(23, 12,14,19);
            return ESP_OK;
        }

        esp_err_t setAxis(float angle, uint8_t velocity, uint8_t acceleration) {
            
            uint8_t buff[4] = {5,5,5,5};

            SPI::write(64,1,buff);
         
            return ESP_OK;
        }

        esp_err_t deinit() {
            return ESP_OK;
        }

    } // namespace stepper

} // namespace motion