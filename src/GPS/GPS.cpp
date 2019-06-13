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
#define uart_GPS_Tx 34
#define uart_GPS_RX

namespace GPS
{
char *readLine(uart_port_t uart) {
	static char line[256];
	int size;
	char *ptr = line;
	while(1) {
		size = uart_read_bytes(uart, (unsigned char *)ptr, 1, portMAX_DELAY);
		if (size == 1) {
			if (*ptr == '\n') {
				ptr++;
				*ptr = 0;
				return line;
			}
			ptr++;
		} 
	} 
} 

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
    uart_param_config(UART_NUM_1,uart_GPS);
    uart_set_pin (uart_GPS,TxPin,RxPin,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);
    uart_driver_install(uart_GPS,2048,2048,10,17,NULL);

}

float getLat()
{
    //Attribut, welches den Lattitude Wert erhält
    float lattitude = 0;
    struct minmea_sentence_gll frame;
    char *uart_data = readLine(UART_NUM_1);
    minmea_parse_gll(&frame,uart_data);

    //Code zum lesen der Lattitude (wird noch geschrieben von Jonas)
    longitude = frame.lattitude;
    return lattitude;
}

float getLong()
{
    //Attribut, welches den Longitude Wert erhält
    float longitude = 0;
    struct minmea_sentence_gll frame;
    char *uart_data = readLine(UART_NUM_1);
    minmea_parse_gll(&frame,uart_data);

    //Code zum lesen der Longitude (wird noch geschrieben von Jonas)
    longitude = frame.longitude;
    return longitude;
}

float getAlt()
{
    //Attribut, welches den Altitude Wert erhält
    float altitude = 0;
    struct minmea_sentence_gga frame;
    char *uart_data = readLine(UART_NUM_1);
    minmea_parse_gga(&frame,uart_data);

    //Code zum lesen der Altitude (wird noc geschrieben von Jonas)
    longitude = frame.altitude;
    return altitude;
}


} // namespace GPS