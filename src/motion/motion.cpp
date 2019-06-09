#include "Motion.h"
namespace motion {

    namespace servo {

        esp_err_t init() {
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
            return ESP_OK;
        }

        esp_err_t setAxis(float angle, uint8_t velocity, uint8_t acceleration) {
            return ESP_OK;
        }

        esp_err_t deinit() {
            return ESP_OK;
        }

    } // namespace stepper

} // namespace motion