//Jonas

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include "stdlib.h"
#include "ctype.h"
#include "math.h"
#include "esp_log.h"
#include <driver/uart.h>

#include "minmea.h"

/**
 * @brief 
 * Code muss noch geschrieben werden (von Jonas)
 * 
 */
namespace GPS
{
char *readLine(uart_port_t uart);
void init(uint8_t TxPin, uint8_t RxPin);
float getLong();
float getLat();
float getAlt();
} // namespace GPS