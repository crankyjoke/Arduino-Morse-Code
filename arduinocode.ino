int ledpin = 13;
int inpin = 12;
bool buttonPressed;
int buttonState = 0;
unsigned long buttonPressTime = 0;
unsigned long buttonReleaseTime = 0;

#include <Arduino.h>

#define SEG_a 2
#define SEG_b 3
#define SEG_c 4
#define SEG_d 5
#define SEG_e 6
#define SEG_f 7
#define SEG_g 8
#define SEG_h 9

unsigned char table[16][8] = {
        { 1, 1, 0, 0, 0, 0, 0, 0 },  // 0
        { 1, 1, 1, 1, 1, 0, 0, 1 },  // 1
        { 1, 0, 1, 0, 0, 1, 0, 0 },  // 2
        { 1, 0, 1, 1, 0, 0, 0, 0 },  // 3
        { 1, 0, 0, 1, 1, 0, 0, 1 },  // 4
        { 1, 0, 0, 1, 0, 0, 1, 0 },  // 5
        { 1, 0, 0, 0, 0, 0, 1, 0 },  // 6
        { 1, 1, 1, 1, 1, 0, 0, 0 },  // 7
        { 1, 0, 0, 0, 0, 0, 0, 0 },  // 8
        { 1, 0, 0, 1, 0, 0, 0, 0 },  // 9
        { 1, 0, 0, 0, 1, 0, 0, 0 },  // A
        { 1, 0, 0, 0, 0, 0, 1, 1 },  // b
        { 1, 1, 0, 0, 0, 1, 1, 0 },  // C
        { 1, 0, 1, 0, 0, 0, 0, 1 },  // d
        { 1, 0, 0, 0, 0, 1, 1, 0 },  // E
        { 1, 0, 0, 0, 1, 1, 1, 0 }   // F
};

char morseCode[26][5] = {
        ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",  // A-J
        "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",    // K-T
        "..-", "...-", ".--", "-..-", "-.--", "--.."                            // U-Z
};

String morseInput = "";
unsigned long lastPressTime = 0;
unsigned long displayTimeout = 5000;

void number(int i) {
    if (i == -1) {
        return;
    }
    pinMode(SEG_a, OUTPUT);
    pinMode(SEG_b, OUTPUT);
    pinMode(SEG_c, OUTPUT);
    pinMode(SEG_d, OUTPUT);
    pinMode(SEG_e, OUTPUT);
    pinMode(SEG_f, OUTPUT);
    pinMode(SEG_g, OUTPUT);
    pinMode(SEG_h, OUTPUT);
    digitalWrite(SEG_a, table[i][7]);
    digitalWrite(SEG_b, table[i][6]);
    digitalWrite(SEG_c, table[i][5]);
    digitalWrite(SEG_d, table[i][4]);
    digitalWrite(SEG_e, table[i][3]);
    digitalWrite(SEG_f, table[i][2]);
    digitalWrite(SEG_g, table[i][1]);
    digitalWrite(SEG_h, table[i][0]);
}

void reset() {

    pinMode(SEG_a, 0);
    pinMode(SEG_b, 0);
    pinMode(SEG_c, 0);
    pinMode(SEG_d, 0);
    pinMode(SEG_e, 0);
    pinMode(SEG_f, 0);
    pinMode(SEG_g, 0);
    pinMode(SEG_h, 0);
}

char alphabet[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int decodeINT(String morse) {
    if (morse == ".----") {
        return 1;
    } else if (morse == "..---") {
        return 2;
    } else if (morse == "...--") {
        return 3;
    } else if (morse == "....-") {
        return 4;
    } else if (morse == ".....") {
        return 5;
    } else if (morse == "-....") {
        return 6;
    } else if (morse == "--...") {
        return 7;
    } else if (morse == "---..") {
        return 8;
    } else if (morse == "----.") {
        return 9;
    } else if (morse == "-----") {
        return 0;
    } else {
        return -1;
    }
}

String decodeMorse(String morse) {
    for (int i = 0; i < 26; i++) {
        if (morse == morseCode[i]) {
            return String(alphabet[i]);
        }
    }
    return "NULL";
}

void setup() {
    pinMode(ledpin, OUTPUT);
    pinMode(inpin, INPUT);
    Serial.begin(9600);
    pinMode(SEG_a, OUTPUT);
    pinMode(SEG_b, OUTPUT);
    pinMode(SEG_c, OUTPUT);
    pinMode(SEG_d, OUTPUT);
    pinMode(SEG_e, OUTPUT);
    pinMode(SEG_f, OUTPUT);
    pinMode(SEG_g, OUTPUT);
    pinMode(SEG_h, OUTPUT);

    reset();
}

void loop() {
    buttonState = digitalRead(inpin);

    if (buttonState == HIGH && !buttonPressed) {
        buttonPressed = true;
        buttonPressTime = millis();
        digitalWrite(ledpin, HIGH);
    } else if (buttonState == LOW && buttonPressed) {
        int sensorValue = analogRead(A0);
        buttonPressed = false;
        buttonReleaseTime = millis();
        digitalWrite(ledpin, LOW);
        unsigned long pressDuration = buttonReleaseTime - buttonPressTime;
        if (pressDuration < sensorValue) {
            Serial.print(".");
            morseInput += ".";
        } else {
            Serial.print("-");
            morseInput += "-";
        }
        lastPressTime = millis();
    }

    if (morseInput.length() > 0 && millis() - lastPressTime > 1750) {
        if (morseInput.length() > 4) {
            Serial.println();
            int dINT = decodeINT(morseInput);
            Serial.println(dINT);
            if (dINT != -1) {
                number(dINT);
            }
            delay(1000);
            reset();
        } else {
            String decodedChar = decodeMorse(morseInput);
            Serial.println();
            Serial.println(decodedChar);
        }
        morseInput = "";
    }


    if (millis() - lastPressTime > displayTimeout) {
        reset();
    }
}
