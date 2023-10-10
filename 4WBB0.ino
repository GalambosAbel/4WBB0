#define DHTPIN 2
#define DHTTYPE DHT22

#include <AbelMainInterface.h>

AbelMainInterface ami(DHTPIN, DHTTYPE);

void setup(){
	ami.setup();
	Serial.begin(115200);
}

void loop(){
	ami.loop();
}