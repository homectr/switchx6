#include "PWMPort.h"
#include <Arduino.h>

#define NODEBUG_PRINT
#include "debug_print.h"

PWMPort::PWMPort(const char* id, unsigned char gpio):Item(id){
    this->gpio = gpio;
    pinMode(gpio,OUTPUT);
    analogWrite(gpio,0);
};

void PWMPort::setDutyCycle(unsigned int dutyCycle){
    if (dutyCycle>100) this->dutyCycle=100;
    else this->dutyCycle = dutyCycle;
    analogWrite(gpio,this->dutyCycle);
    if (onChange) onChange(this);
};

