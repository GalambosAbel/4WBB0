#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define RXPIN D7
#define TXPIN D8

#define MESSAGELENGTH 9
#define STARTSYMBOL 255

SoftwareSerial insideSerial(RXPIN, TXPIN);

float t = 0;
float h = 0;
int knob = 0;


const char* apSSID     = "ESP8266-Access-Point";
const char* apPassword = "4wbb01111";
const char* localSSID = "Fabrice";
const char* localPassword = "4wbb01111";
IPAddress IP;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
IPAddress espIP(192,168,43,234);
IPAddress apIP(192,168,43,81);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 10000;  

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  return String();
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

#define EEPROM_SIZE 512
#define SAVE_START_ADDRESS 0
#define SAVE_END_ADDRESS 511

String front = ""; //front word
String back = ""; //back word

bool head = true; //write to front or back word next

//loads the front or back word from memory
String loadWord(bool start = true) {
    String word = "";
    int dir = start ? 1 : -1;
    int address = start ? SAVE_START_ADDRESS : SAVE_END_ADDRESS;

    int length = EEPROM.read(address); //get word length
    for (int i = 0; i < length; i++) {
        word += (char)EEPROM.read(address + (i + 1) * dir); //read each character
    }

    return word;
}

void saveWord(String word, bool start = true) {
    if(word.length() > 255) { //we couldn't save a wordlength greater than 255, but 2*255+2 = 512, fits in memory
        Serial.println("Can't save, too long");
        return;
    }

    int dir = start ? 1 : -1;
    int address = start ? SAVE_START_ADDRESS : SAVE_END_ADDRESS;

    EEPROM.write(address, word.length()); //save word length
    for (int i = 0; i < word.length(); i++) {
        EEPROM.write(address + (i + 1) * dir, word[i]); //save each character in order
    }

    EEPROM.commit();
}

void setup() {
    
    delay(1000); //this is sometimes needed, I don't know why TF
    Serial.begin(115200);
    EEPROM.begin(EEPROM_SIZE);

    front = loadWord();
    back = loadWord(false);

    pinMode(LED_BUILTIN, OUTPUT); // set up LED as output

    pinMode(RXPIN, INPUT);
    pinMode(TXPIN, OUTPUT);

    insideSerial.begin(9600);

  // Configure static AP IP address:
  //if (!WiFi.softAPConfig(apIP, gateway, subnet)) {
  //  Serial.println("Configuration IP failed to configure.");
  //}

    // Configures static local IP address
    if (!WiFi.config(espIP, gateway, subnet)) {
        Serial.println("ESP IP failed to configure.");
    }

    // Connect to wifi network:
    WiFi.begin(localSSID, localPassword);
    while (WiFi.status() != WL_CONNECTED) { // if no connection has been found yet...
        delay(1000); // wait a sec
        Serial.println("."); // print a dot.
    } // try again

    // Print ESP8266 Local IP address
    
    espIP = WiFi.localIP();
    Serial.print("ESP8266 IP address:");
    Serial.println(espIP);

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", "online");
    });
    server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", String(t).c_str());
    });

    // Start server
    server.begin();
}

void loop() {
    bool read = readSerial();

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval && WiFi.status() == WL_CONNECTED) {
        digitalWrite(LED_BUILTIN, LOW); // set LED to low
        delay(200); // wait for 200 ms
        digitalWrite(LED_BUILTIN, HIGH); // set LED to high
        // save the last time you updated the DHT values
        previousMillis = currentMillis;

        // Print info on state:
        Serial.print("AP IP address: ");
        Serial.println(apIP);
        Serial.print("ESP8266 IP address: ");
        Serial.println(espIP);
        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.print("; Humidity: ");
        Serial.println(h);
    } else if (currentMillis - previousMillis >= interval && WiFi.status() != WL_CONNECTED) {
        // try to re-establish connection...
        previousMillis = currentMillis;
        Serial.println("."); // print a dot.
    }
    
    String w = "";
    while (Serial.available() > 0) {
        w += (char)Serial.read();
    }
    if (w != "") {
        (head ? front : back) = w;
        Serial.print("Saving: ");
        Serial.println(w);
        saveWord(w, head);
        head = !head;
    }
    //Serial.print(front);
    //Serial.print(" || ");
    //Serial.println(back);
}