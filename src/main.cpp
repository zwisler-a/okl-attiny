#include <Arduino.h>
#include <SoftwareSerial.h>
#include "tinysnore.h"

const char *startupLEDPattern = "10111";
const char *sleepLEDPattern = "1010111";
const char *unknownCommandPattern = "101010111";

const char END_OF_COMMAND = ';';

const byte rxPin = PIN_PB3;
const byte txPin = PIN_PB4;
const byte ledPin = LED_BUILTIN;
const byte espPin = PIN_PB0;
const byte sensorPin = A1;

SoftwareSerial softwareSerial(rxPin, txPin); // RX, TX


void playSequence(const char *pattern) {
    for (int i = 0; pattern[i] != '\0'; ++i) {
        if (pattern[i] == '1') {
            digitalWrite(ledPin, HIGH);
        } else if (pattern[i] == '0') {
            digitalWrite(ledPin, LOW);
        }
        delay(100);
    }
    digitalWrite(ledPin, LOW);
}


void setup() {
    softwareSerial.begin(4800);
    pinMode(txPin, OUTPUT); // Set TX pin as OUTPUT
    pinMode(ledPin, OUTPUT);
    pinMode(espPin, OUTPUT);
    pinMode(sensorPin, INPUT);
    while (Serial.available()) { Serial.read(); }
    playSequence(startupLEDPattern);
    digitalWrite(espPin, HIGH);
}

void enterSleep(long duration_in_sec) {
    if (duration_in_sec <= 1) {
        return;
    }
    playSequence(sleepLEDPattern);
    digitalWrite(espPin, LOW);
    snore((duration_in_sec * 1000) - 2500);
    digitalWrite(espPin, HIGH);
    playSequence(sleepLEDPattern);
    while (Serial.available()) { Serial.read(); }
}

void loop() {
    if (softwareSerial.available() > 0) {
        String cmd = softwareSerial.readStringUntil(END_OF_COMMAND);
        if (cmd.startsWith("S:")) {
            String durationStr = cmd.substring(2);
            if (durationStr.length() < 1) {
                playSequence(unknownCommandPattern);
            }
            int duration = atol(durationStr.c_str());
            softwareSerial.print("#S:");
            softwareSerial.print(duration);
            softwareSerial.print(END_OF_COMMAND);
            enterSleep(duration);
            return;
        }

        if (cmd.startsWith("H")) {
            softwareSerial.print("#H:");
            softwareSerial.print(analogRead(sensorPin));
            softwareSerial.print(END_OF_COMMAND);
            return;
        }

        playSequence(unknownCommandPattern);
        softwareSerial.print("!");
        softwareSerial.print(cmd);
        softwareSerial.print(END_OF_COMMAND);
        while (Serial.available()) { Serial.read(); }
    }
}
