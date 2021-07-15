#pragma once

#include <Homie.h>
#include "List.h"
#include "Item.h"
#include "Switch.h"
#include "Sequence.h"

#define NUMBER_OF_ITEMS 6

class Thing {
    public:
        Item* items[NUMBER_OF_ITEMS];

        List<GPIOSwitch> switches;
        ListIterator<GPIOSwitch> *switchIterator = NULL; // helper for the loop method
        Sequence<unsigned char> sequence = Sequence<unsigned char>("switches");


        HomieNode homieDevice = HomieNode("device", "Device", "device");
        HomieNode homieSwitches = HomieNode("switches", "Switches", "switch");
        HomieNode homiePWM = HomieNode("pwm", "PWM Ports", "pwm");
        HomieSetting<const char*> *itemCfg[NUMBER_OF_ITEMS];

    protected:
        unsigned long seqStatusUpdatedOn = 0;
        unsigned long aliveTimer = 0;

        bool configured = false;

        Item* createItem(const char* cfg);
        Switch* createSwitch(const char* cfg);

    public:
        Thing();
        void setup(); // call after Homie.setup()
        void loop();
        bool isConfigured(){return configured;};

};

