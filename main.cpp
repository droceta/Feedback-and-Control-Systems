#include <Arduino.h>

const int ledPin = 12;

void setup() {
    pinMode(ledPin, OUTPUT);
    Serial.begin(9600);
    Serial.println("LED Controller Ready. Send '1' to turn ON, '0' to turn OFF.");
}

void loop() {
    if (Serial.available() > 0) {
        char command = Serial.read();

        if (command == '1') {
            digitalWrite(ledPin, HIGH);
            Serial.println("LED ON");
        } else if (command == '0') {
            digitalWrite(ledPin, LOW);
            Serial.println("LED OFF");
        }
    }
}