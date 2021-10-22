#pragma once

#include <Homie.h>
#include "List.h"
#include "Item.h"
#include "Switch.h"
#include "Sequence.h"
#include "PWMPort.h"
#include "BLEScanner.h"

#define NUMBER_OF_ITEMS 1

class Thing {
    public:
        Item* items[NUMBER_OF_ITEMS];

        List<GPIOSwitch> switches;
        Sequence<unsigned char> sequence = Sequence<unsigned char>("switches");

        List<PWMPort> pwm;
        List<BLEScanner> bles;

        HomieNode homieDevice = HomieNode("thing", "Thing", "thing");
        HomieNode homieSwitches = HomieNode("switch", "Switches", "switch");
        HomieNode homiePWM = HomieNode("pwm", "PWM Ports", "pwm");
        HomieNode homieBLES = HomieNode("bles", "BLE Scanners", "bles");
        HomieSetting<const char*> *itemCfg[NUMBER_OF_ITEMS];

    protected:
        unsigned long seqStatusUpdatedOn = 0;
        unsigned long aliveTimer = 0;

        bool configured = false;

        Item* createItem(const char* cfg);
        Switch* createSwitch(const char* cfg);
        PWMPort* createPWM(const char* cfg);
        BLEScanner* createBLEScanner(const char* cfg);

    public:
        Thing();
        void setup(); // call after Homie.setup()
        void loop();
        bool isConfigured(){return configured;};

};

