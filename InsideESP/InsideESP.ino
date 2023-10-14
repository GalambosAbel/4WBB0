#include <SoftwareSerial.h>

#include <Arduino.h>

#define RXPIN D7
#define TXPIN D8

#define MESSAGELENGTH 9
#define STARTSYMBOL 255

SoftwareSerial insideSerial(RXPIN, TXPIN);

float t = 0;
float h = 0;
int knob = 0;

void setup() {
    Serial.begin(115200);

    pinMode(RXPIN, INPUT);
    pinMode(TXPIN, OUTPUT);

    insideSerial.begin(9600);
}
int fff = 0;

void loop() {
    bool read = readSerial();
    Serial.print(t);
    Serial.print(' ');
    Serial.print(h);
    Serial.print(' ');
    Serial.print(knob);
    Serial.print(' ');
    Serial.println(read);
}

bool readSerial() {
    //full message is here, and first symbol is correct
    //otherwise it just burns first symbol
    bool read = false;
    if(insideSerial.available() >= MESSAGELENGTH && insideSerial.read() == STARTSYMBOL) {
        t = readFloat();
        h = readFloat();
        knob  = readUInt();
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

void sendSerial(int color) {
    //mark start
    insideSerial.write(255);

    sendUInt(color);
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