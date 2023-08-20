#include <Arduino.h>
#include <SoftwareSerial.h>

const char *startupLEDPattern = "10101";
const char *unknownCommandPattern = "1010111";

const byte rxPin = 3;
const byte txPin = 4;
const byte ledPin = 1;
const byte espPin = 2;

SoftwareSerial softwareSerial(rxPin, txPin); // RX, TX


void playSequence(const char *pattern) {
    for (int i = 0; pattern[i] != '\0'; ++i) {
        if (pattern[i] == '1') {
            digitalWrite(ledPin, HIGH);
        } else if (pattern[i] == '0') {
            digitalWrite(ledPin, LOW);
        }
        delay(500);
    }
    digitalWrite(ledPin, LOW);
}


void setup() {
    softwareSerial.begin(4800);
    pinMode(txPin, OUTPUT); // Set TX pin as OUTPUT
    pinMode(ledPin, OUTPUT);
    pinMode(espPin, OUTPUT);
    digitalWrite(espPin, HIGH);
    playSequence(startupLEDPattern);
}

void enterSleep(long duration) {
    digitalWrite(espPin, LOW);
    for (long i = 0; i < duration / 2; i++) {
        digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
        delay(1900);
    }
    digitalWrite(espPin, HIGH);
}

void loop() {
    if (softwareSerial.available() > 0) {
        digitalWrite(ledPin, HIGH);
        String cmd = softwareSerial.readStringUntil('\n');
        digitalWrite(ledPin, LOW);
        if (cmd.startsWith("S")) {
            int duration = atol(cmd.substring(2).c_str());
            softwareSerial.print("#S:");
            softwareSerial.println(duration);
            enterSleep(duration);
            return;
        }
        if (cmd.startsWith("H")) {
            softwareSerial.println("#H");
            return;
        }
        playSequence(unknownCommandPattern);

    }
}
