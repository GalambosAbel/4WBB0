#include <EEPROM.h>
#include <Arduino.h>

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
    //delay(1000); //this is sometimes needed, I don't know why TF
    Serial.begin(115200);
    EEPROM.begin(EEPROM_SIZE);

    front = loadWord();
    back = loadWord(false);
}

void loop() {
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
    Serial.print(front);
    Serial.print(" || ");
    Serial.println(back);
    delay(500);
}