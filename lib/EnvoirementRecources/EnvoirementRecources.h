#include <stdint.h>
#include <stdio.h>

//Dieses File stellt eine Klasse zur Verfügung, welche Parameter zum Drohnenstatus 
//sowie zum Status des Antenna Trackers beinhaltet
//Methoden die mit einem "Warning!" versehen sind, sollen nur von bestimmten Orten heraus aufgerufen werden

class Recources
{
    private:
    //Attribute, welche Parameter der Drohne beschreiben:
        float lattitude;
        float longitude;
        float altitude;
        uint8_t DrohneVoltage;

    //Attribute welche Parameter des Antenna Trackers beschreiben:
        float Batt1Voltage;
        float Batt2Voltage;
        float XAngle;
        float YAngle;
        float trackerLat;
        float trackerLongi;

    public:
        Recources();
        // Methoden zum lesen der Drohnen Parameter
        //Diese Methoden können von überall her aufgerufen werden!
        float readLatt();
        float readLong();
        float readAlt();
        uint8_t readDrohneVolt();

        //Methoden zum Setzen der Parameter
        
        //Warning! Sollte nur von der Funktion "esp_spp_cb" aufgerufen werden
        float setLatt();
        //Warning! Sollte nur von der Funktion "esp_spp_cb" aufgerufen werden
        float setLong();
        //Warning! Sollte nur von der Funktion "esp_spp_cb" aufgerufen werden
        float setAlt();
        uint8_t SetDrohneVolt();


};