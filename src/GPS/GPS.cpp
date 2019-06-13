#include "GPS.h"


#include <stdint.h>
#include "freertos/FreeTROS.h"
#include "freetros/task.h"
#include "stdlib.h"
#include "ctype.h"
#include "math.h"
#include "esp_log.h"

#include "minmea.h"

//Pins defines
#define uart_GPS_Tx
#define uart_GPS_RX

namespace GPS
{
//Jonas
void init(uint8_t TxPin, uint8_t RxPin)
{
    //Code zum Initialisieren des GPS (wird noch geschrieben von Jonas)
    uart_config_t uart_GPS;
    uart_GPS.baud_rate = 9600;
    uart_GPS.data_bits = UART_DATA
	uart_GPS.parity = UART_PARITY_DISABLE;
	uart_GPS.stop_bits = UART_STOP_BITS_1;
    uart_GPS.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_GPS.rx_flow_ctrl_thresh = 120;
    uart_set_pin (uart_GPS,);
    uart_driver_install(uart_GPS,);

}
//Jonas
float getLat()
{
    //Attribut, welches den Lattitude Wert erhält
    float lattitude = 0;
    struct minmea_sentence_gga frame;

    //Code zum lesen der Lattitude (wird noch geschrieben von Jonas)
    longitude = frame.lattitude.value;
    return lattitude;
}
//Jonas
float getLong()
{
    //Attribut, welches den Longitude Wert erhält
    float longitude = 0;
    struct minmea_sentence_gga frame;

    //Code zum lesen der Longitude (wird noch geschrieben von Jonas)
    longitude = frame.longitude.value;
    return longitude;
}
//Jonas
float getAlt()
{
    //Attribut, welches den Altitude Wert erhält
    float altitude = 0;
    struct minmea_sentence_gga frame;

    //Code zum lesen der Altitude (wird noc geschrieben von Jonas)
    longitude = frame.altitude.value;
    return altitude;
}


} // namespace GPS