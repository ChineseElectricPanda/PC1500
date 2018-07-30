#include "PC1500.h"
#include "Arduino.h"

volatile StatusFlag PC1500::statusFlags = static_cast<StatusFlag>(0);
volatile uint8_t PC1500::numStatusFlagsRead = 0;
volatile bool PC1500::isReading = false;

uint32_t PC1500::bitsToWrite = 0;
volatile uint8_t PC1500::numBitsWritten = 0;
volatile bool PC1500::isWriting = false;

volatile bool PC1500::synced = false;

void PC1500::init(uint8_t clockPin, uint8_t dataPin) {
    PC1500::clockPin = clockPin;
    PC1500::dataPin = dataPin;
    synced = false;
    attachInterrupt(digitalPinToInterrupt(clockPin), syncIsr, RISING);
    while(!synced) { }
    attachInterrupt(digitalPinToInterrupt(clockPin), readWriteIsr, CHANGE);
}

StatusFlag PC1500::getStatus() {
    while(isReading) {
        delay(1);
    }
    return statusFlags;
}

void writeKey(char c) {
    while(isWriting) {
        delay(1);
    }
    // Repeat the character 4 times;
    bitsToWrite = c << 24 | c << 16 | c << 8 | c;
}


void PC1500::syncIsr() {
    delayMicroseconds(1500);
    if(digitalRead(clockPin) == HIGH) {
        synced = true;
    }
}

void PC1500::readWriteIsr() {
    delayMicroseconds(50);
    if(digitalRead(clockPin) == LOW) {
        if(isWriting) {
            write();
            if(numBitsWritten == 32) {
                isWriting = false;
            }
        }
    } else {
        if(isReading) {
            read();
            if(numStatusFlagsRead == 16) {
                isReading == false;
            }
        }
    }
}

void PC1500::read() {
    numStatusFlagsRead = numStatusFlagsRead % 16;
    pinMode(dataPin, INPUT);
    delayMicroseconds(100);
    statusFlags <<= 1;
    statusFlags |= digitalRead(dataPin);
    numStatusFlagsRead++;
}

void PC1500::write() {
    numBitsWritten = numBitsWritten % 32;
    pinMode(dataPin, OUTPUT);
    delayMicroseconds(100);
    digitalWrite(dataPin, bitsToWrite & (0x1 << (31 - numBitsWritten)) ? HIGH : LOW);
    numBitsWritten++;
}