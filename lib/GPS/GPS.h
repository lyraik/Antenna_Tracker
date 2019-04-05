//Dieses File beinhaltet API's für GPS Peripherien
#include <iostream>

//Jonas
class GPS
{
    public:
        GPS(uint8_t TxPin, uint8_t RxPin);
        float getLong();
        float getLat(); 
        float getAlt();
};