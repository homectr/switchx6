#include "Switch.h"
#include <Arduino.h>

#define NODEBUG_PRINT
#include "debug_print.h"

Switch::Switch(const char* id){
    this->id = id;
}

void Switch::on(){
    status = true;
    onSince = millis();
    if (onOn) onOn(this);
    DEBUG_PRINT("[Switch-%s] ON millis=%lu momentary=%lu\n",id,onSince,momentaryTimeout);
}

void Switch::off(){
    status = false;
    if (onOff) onOff(this);
    DEBUG_PRINT("[Switch-%s] OFF millis=%lu\n",id,millis());
}

void Switch::loop(){
    if (status && momentaryTimeout > 0 && millis()-onSince > momentaryTimeout) off();
}

GPIOSwitch::GPIOSwitch(unsigned char gpioPin, const char* id, unsigned char inverse):Switch(id){
    pin = gpioPin;
    this->inverse = inverse;
}

void GPIOSwitch::on(){
    Switch::on();
    digitalWrite(pin,!inverse);
    DEBUG_PRINT("[GPIO-%d] on\n",pin);

}

void GPIOSwitch::off(){
    digitalWrite(pin,inverse);
    Switch::off();
    DEBUG_PRINT("[GPIO-%d] off\n",pin);
}