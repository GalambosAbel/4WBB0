#include <DHTReader.h>
#include <LEDController.h>

class AbelMainInterface{
public:
    AbelMainInterface(uint8_t dht_pin, uint8_t dht_type);
    void setup();
    void loop();
    float temperature;
    float humidity;

private:
    DHTReader dhtReader;
    LEDController ledController;
};