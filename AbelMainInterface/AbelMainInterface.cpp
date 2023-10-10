#include <AbelMainInterface.h>

AbelMainInterface::AbelMainInterface(uint8_t dht_pin, uint8_t dht_type) : dhtReader(dht_pin, dht_type){
}

void AbelMainInterface::setup(){
    dhtReader.setup();
    ledController.setup();
}

void AbelMainInterface::loop() {
    dhtReader.loop();
    ledController.loop();
    
    temperature = dhtReader.temperature;
    humidity = dhtReader.humidity;
}