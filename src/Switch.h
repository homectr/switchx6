#pragma once

#include "Item.h"

class Switch: public Item {
    using event_cb_t = void(*)(Switch*);
    protected:
        unsigned int momentaryTimeout = 0;  // non-zero means momentary switch
        unsigned long onSince = 0;
        unsigned char status = 0;

        event_cb_t onOn;
        event_cb_t onOff;

    public:
        Switch(const char* id);
        virtual void on();
        virtual void off();
        bool isOn(){return status;};
        bool isMomentary(){return momentaryTimeout>0;};
        
        void setCbOn(event_cb_t cb){onOn = cb;};
        void setCbOff(event_cb_t cb){onOff = cb;};
        void setMomentary(unsigned int timeout=10){momentaryTimeout=timeout;};

        // from Item
        virtual void loop();

};

class GPIOSwitch: public Switch {
    protected:
        unsigned char pin;
        unsigned char inverse;

    public:
        GPIOSwitch(unsigned char gpioPin, const char* id, unsigned char inverse=0);

        // from Switch
        virtual void on() override;
        virtual void off() override;
        
};