#pragma once

#include <Homie.h>
#include <Syslog.h>
#include "List.h"
#include "Item.h"
#include "Switch.h"
#include "Sequence.h"
#include "PWMPort.h"
#include "Logger.h"

#define NUMBER_OF_ITEMS 6

class Thing:public Logger {
    public:
        Item* items[NUMBER_OF_ITEMS];

        List<GPIOSwitch> switches;
        Sequence<unsigned char> sequence = Sequence<unsigned char>("switches");

        List<PWMPort> pwm;

        HomieNode homieDevice = HomieNode("thing", "Thing", "thing");
        HomieNode homieSwitches = HomieNode("switch", "Switches", "switch");
        HomieNode homiePWM = HomieNode("pwm", "PWM Ports", "pwm");
        HomieSetting<const char*> *itemCfg[NUMBER_OF_ITEMS];  

    protected:
        unsigned long seqStatusUpdatedOn = 0;
        unsigned long aliveTimer = 0;

        bool configured = false;

        Item* createItem(const char* cfg);
        Switch* createSwitch(const char* cfg);
        PWMPort* createPWM(const char* cfg);

    public:
        Thing();
        void setup(); // call after Homie.setup()
        void loop();
        bool isConfigured(){return configured;};
};

