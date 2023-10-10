#include <DHT.h>

class DHTReader {
public:
    DHTReader(uint8_t pin, uint8_t type);
    float temperature;
    float humidity;
    void setup();
    void loop();

private:
    DHT dht;
};