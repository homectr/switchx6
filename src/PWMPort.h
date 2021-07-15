#pragma once

#include "Item.h"

class PWMPort : public Item {
    using event_cb_t = void(*)(PWMPort*);

    protected:
        unsigned char gpio;
        unsigned int dutyCycle;
        event_cb_t onChange;

    public:
        PWMPort(const char* id, unsigned char gpio);
        void setDutyCycle(unsigned int dutyCycle);
        void off(){setDutyCycle(0);};
        unsigned int getDutyCycle(){return dutyCycle;};
        void setCbOnChange(event_cb_t onChange){this->onChange = onChange;};
};