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

#ifndef PC1500_h
#define PC1500_h

class PC1500 {

public:
    enum StatusFlag
    {
        ZONE_1,
        ZONE_2,
        ZONE_3,
        ZONE_4,
        ZONE_5,
        ZONE_6,
        ZONE_7,
        ZONE_8,
        READY,
        ARMED,
        MEMORY,
        BYPASS,
        TROUBLE,
        BEEP
    }

    PC1500();

    void sync();
    StatusFlag readStatus();
    void write(char c);

private:
    uint16_t statusFlags;
    uint8_t numStatusFlagsRead;
    bool isReading;

    uint32_t bitsToWrite;
    uint8_t numBitsWritten;
    bool isWriting;

    bool synced;

    void syncIsr();
    void readIsr();
    void writeIsr();
};

#endif