#include <SoftwareSerial.h>

#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 12
#define DHTTYPE DHT22

#define REDPIN 4
#define GREENPIN 3
#define BLUEPIN 2

#define RXPIN 5
#define TXPIN 6

#define KNOBPIN A0


#define MESSAGELENGTH 3
#define STARTSYMBOL 255

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial insideSerial(RXPIN, TXPIN);

float t = 0;
float h = 0;
int knob = 0;


int color = 0;
int r = 0, g = 0, b = 0;

void setup() {
    Serial.begin(115200);
    dht.begin();
    pinMode(REDPIN, OUTPUT);
    pinMode(GREENPIN, OUTPUT);
    pinMode(BLUEPIN, OUTPUT);

    pinMode(RXPIN, INPUT);
    pinMode(TXPIN, OUTPUT);

    pinMode(KNOBPIN, INPUT);

    insideSerial.begin(9600);
}

void loop() {
    readDHT();
    readKnob();
    sendSerial();
}

void readDHT() {
    t = dht.readTemperature();
    h = dht.readHumidity();
}

void setLED(int r, int g, int b) {
    analogWrite(REDPIN, r);
    analogWrite(GREENPIN, g);
    analogWrite(BLUEPIN, b);
}

void readKnob() {
    knob = analogRead(KNOBPIN);
}


bool readSerial() {
    //full message is here, and first symbol is correct
    //otherwise it just burns first symbol
    bool read = false;
    if(insideSerial.available() >= MESSAGELENGTH && insideSerial.read() == STARTSYMBOL) {
        if(color == 0) {
            color++;
            r = readUInt();
        }else if (color == 1) {
            color++;
            g = readUInt();
        }else if (color == 2) {
            color = 0;
            b = readUInt();
        }
        read = true;
    }
    return read;
}

float readFloat() {
    float sign = insideSerial.read() * 2 - 1;
    float f = (float)insideSerial.read();
    f += (float)insideSerial.read() / 100.00;
    f *= sign;
    return f;
}

int readUInt() {
    int i = insideSerial.read() * 100;
    i += insideSerial.read();
    return i;
}

void sendSerial() {
    //mark start
    insideSerial.write(255);

    sendFloat(t);
    sendFloat(h);
    sendUInt(knob);
}

void sendFloat(float f) { //3bytes
    //<0 -> negative, 1 -> positive ; integer part ; first 2 digits part>
    insideSerial.write(f < 0 ? 0 : 1);
    insideSerial.write((int)abs(f));
    insideSerial.write(((int)abs(f * 100)) % 100);
}

void sendUInt(int i) { //2 bytes
    //<mod 10000 ; mod 100>
    i = abs(i);
    insideSerial.write(i / 100);
    insideSerial.write(i % 100);
}