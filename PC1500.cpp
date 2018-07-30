#include "PC1500.h"
#include "Arduino.h"

volatile StatusFlag PC1500::statusFlags = static_cast<StatusFlag>(0);
volatile uint8_t PC1500::numStatusFlagsRead = 0;
volatile bool PC1500::isReading = false;

uint32_t PC1500::bitsToWrite = 0;
volatile uint8_t PC1500::numBitsWritten = 0;
volatile bool PC1500::isWriting = false;

volatile bool PC1500::synced = false;

uint8_t PC1500::clockPin = 0;
uint8_t PC1500::dataPin = 0;

void PC1500::init(uint8_t clockPin, uint8_t dataPin) {
    PC1500::clockPin = clockPin;
    PC1500::dataPin = dataPin;
    synced = false;
    attachInterrupt(digitalPinToInterrupt(clockPin), syncIsr, RISING);
    while(!synced) {
        delay(0);
    }
    attachInterrupt(digitalPinToInterrupt(clockPin), readWriteIsr, CHANGE);
}

StatusFlag PC1500::getStatus() {
    while(isReading) {
        delay(0);
    }
    return statusFlags;
}

void PC1500::writeKey(char c) {
    while(isWriting) {
        delay(0);
    }
    uint32_t code = charToKeypad(c);
    // Repeat the character 4 times;
    bitsToWrite = code << 24 | code << 16 | code << 8 | code;
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

uint8_t PC1500::charToKeypad(char c) {
    switch (c) {
        case '1': return 0x41;
        case '2': return 0x21;
        case '3': return 0x11;
        case '4': return 0x42;
        case '5': return 0x22;
        case '6': return 0x12;
        case '7': return 0x44;
        case '8': return 0x24;
        case '9': return 0x14;
        case '0': return 0x28;
        case '*': return 0x48;
        case '#': return 0x18;
        case 'F': return 0xC0;
        case 'E': return 0xA0;
        case 'P': return 0x90;
    }
    return 0x00;
}