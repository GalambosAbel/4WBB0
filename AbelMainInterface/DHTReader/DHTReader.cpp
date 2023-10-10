#include <DHTReader.h>

DHTReader::DHTReader(uint8_t pin, uint8_t type) : dht(pin, type){
}

void DHTReader::setup(){
    dht.begin();
}

void DHTReader::loop(){
	delay(2000);
	temperature = dht.readTemperature();
	isnan(temperature) ? Serial.print('t') : Serial.print(temperature);
	Serial.print(' ');
	
	humidity = dht.readHumidity();
	isnan(humidity) ? Serial.print('h') : Serial.print(humidity);
	Serial.println();
}