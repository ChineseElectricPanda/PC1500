/**
 * Controller for interfacing with PC1500 Alarm system
 * Protocol References: 
 * https://www.avrfreaks.net/sites/default/files/forum_attachments/pc1500%20Keybus%20Layout.jpg
 * https://pastebin.com/sN7JnJHM

 * Notes:
 * - Clock is around 1KHz
 * - Clock (yellow) is normally HIGH
 * - Data (green) is normally LOW
 * - A 'packet' is sent every 20ms or so, consisting of
 *   - 16 clock pulses (beginning with a FALLING edge)
 *   - On FALLING edges, keypad sends data
 *   - On RISING edges, master sends data (LED flags)
 *   - There are 8 bits of data sent by the keypad, so it is sent twice in the packet
 *   - There are 16 flag bits sent by the master, each corresponding to an LED on the keypad
 * - Can only send a new key every 2 packets to prevent keys being dropped
 *   - Every key sent is repeated 4 times

 * LED flags (MSB first)
 * Zone 1
 * Zone 2
 * Zone 3
 * Zone 4
 * Zone 5
 * Zone 6
 * Zone 7
 * Zone 8
 * Ready
 * Armed
 * Memory
 * Bypass
 * Trouble
 * ???
 * ???
 * Beep

 * Keypad matrix output (MSB first):
 * Corresponding Row/Col is 0, e.g. 10111110 is 1
 * RCCCRRRR
 * Columns| 0 | 1 | 2 | 
 *        |---|---|---|---
 *        | 1 | 2 | 3 | 4
 *        |---|---|---|---
 *        | 4 | 5 | 6 | 3
 *        |---|---|---|---
 *        | 7 | 8 | 9 | 2
 *        |---|---|---|---
 *        | * | 0 | # | 1
 *        |---|---|---|---
 *        | F | E | P | 0
 *        |---|---|---|---
 *                     Rows
 */

#include <stdint.h>

#ifndef PC1500_h
#define PC1500_h

enum StatusFlag
{
    ZONE_1  = 1 << 15,
    ZONE_2  = 1 << 14,
    ZONE_3  = 1 << 13,
    ZONE_4  = 1 << 12,
    ZONE_5  = 1 << 11,
    ZONE_6  = 1 << 10,
    ZONE_7  = 1 << 9,
    ZONE_8  = 1 << 8,
    READY   = 1 << 7,
    ARMED   = 1 << 6,
    MEMORY  = 1 << 5,
    BYPASS  = 1 << 4,
    TROUBLE = 1 << 3,
    BEEP    = 1 << 0
};

class PC1500 {

public:
    static void init(uint8_t clockPin, uint8_t dataPin);
    static StatusFlag getStatus(void);
    static void writeKey(char c);

private:
    static uint8_t clockPin, dataPin;

    static volatile StatusFlag statusFlags;
    static volatile uint8_t numStatusFlagsRead;
    static volatile bool isReading;

    static uint32_t bitsToWrite;
    static volatile uint8_t numBitsWritten;
    static volatile bool isWriting;

    static volatile bool synced;

    static void syncIsr(void);
    static void readWriteIsr(void);

    static void read(void);
    static void write(void);

};

#endif