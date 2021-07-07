#pragma once

#include <Homie.h>
#include "List.h"
#include "Switch.h"
#include "Sequence.h"

const unsigned int NUMBER_OF_SWITCHES = 6;

class Device {

    public:
        List<GPIOSwitch> switches;
        ListIterator<GPIOSwitch> *switchIterator = nullptr; // helper for the loop method
        Sequence<unsigned char> sequence = Sequence<unsigned char>("switches");
        HomieNode homieDevice = HomieNode("device", "Device", "device");
        HomieNode homieSwitches = HomieNode("switches", "Switches", "switch");
        HomieSetting<const char*> *scfg[NUMBER_OF_SWITCHES];

    protected:
        unsigned long seqStatusUpdatedOn = 0;
        unsigned long aliveTimer = 0;

    public:
        Device();
        void setup(); // call after Homie.setup()
        void loop();

};