#pragma once

#include <SoftwareSerial.h>
#include "Item.h"
#include "List.h"

#define BLE_SCAN_INTERVAL       10000
#define BLE_SCAN_BUFFER_SIZE    200

struct DISARec {
    unsigned char mac[6];
    unsigned char devType;
    char rssi;
    unsigned char flags;
    char localName[17]
    unsigned char dataLength;
    unsigned char data[8];
};

class BLEScanner: public Item {
    protected:
        unsigned char group;  // group beacons should belong to, non-group beacons will be filtered
        SoftwareSerial* ss = NULL;
        unsigned long speed = 9600;
        unsigned char running = 0;
        List<char> beacons;  // list of found beacons

        unsigned long lastScanStart = 0; // when last scan started
        unsigned char buffer[BLE_SCAN_BUFFER_SIZE];
        unsigned int buffUsed = 0;
        unsigned int lastUsed = 0; // used buffer before the last processing

        #ifndef NODEBUG_PRINT
        unsigned long lastAlive=0;
        #endif

        void atCommand(const char* cmd);
        size_t processBuffer(unsigned char* buff, size_t size);
        size_t shiftBuffer(unsigned char* buff, size_t size, size_t shiftBy);

    public:
        BLEScanner(const char* id, unsigned char rxPin, unsigned char txPin, unsigned long speed, unsigned char group);
        void begin();
        virtual void loop() override;
};