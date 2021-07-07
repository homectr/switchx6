#include "Device.h"
#include <Arduino.h>
#include "handlers.h"

#define NODEBUG_PRINT
#include "debug_print.h"

const unsigned char GPIOS[NUMBER_OF_SWITCHES] = { 13, 12, 14, 16, 2, 4 };

Device::Device(){

    DEBUG_PRINT("Dev-List %X\n",switches);

    for (uint8_t i = 0; i < NUMBER_OF_SWITCHES; i++) {

        // initialize GPIO
        pinMode(GPIOS[i], OUTPUT);
        digitalWrite(GPIOS[i], LOW);

        // create switch for GPIO
        const char *id = strdup(String(i+1).c_str());
        GPIOSwitch *s = new GPIOSwitch(GPIOS[i],id);
        s->setCbOff(handleSwitchOff);
        s->setCbOn(handleSwitchOn);
        switches.add(id,s);

        // configure Homie property for the switch
        homieSwitches.advertise(id).setDatatype("boolean").setRetained(false).settable();

        // create configuration setting for each switch
        scfg[i] = new HomieSetting<const char*>(id, strdup(String(String("Configuration for switch #")+id).c_str()));
        
        // all switches are set as non-momentary by default
        scfg[i]->setDefaultValue("nm");
    }

    // create properties for device
    homieDevice.advertise("seq").setDatatype("string").settable(sequenceHandler);
    homieDevice.advertise("seqStatus").setDatatype("integer");
    homieDevice.advertise("cmd").setDatatype("string").settable(cmdHandler);

    sequence.handleOnStart(handleSequenceStart);
    sequence.handleOnStop(handleSequenceStop);
    sequence.handleOnStepStart(handleStepStart);
    sequence.handleOnStepStop(handleStepStop);

}

void Device::setup(){
    DEBUG_PRINT("[Device] SETUP\n");
    // process Homie settings
    for (int i=0; i<NUMBER_OF_SWITCHES;i++){
        DEBUG_PRINT("i=%d",i);
        const char* s = scfg[i]->get();
        DEBUG_PRINT(" cfg=%s",s);
        int t = 0;
        if (*s == 'm'){ // momentary
            t = atoi(s+2);
            if (t<0) t=10;
        }
        DEBUG_PRINT(" t=%d",t);
        Switch* sw = switches.get(String(i+1).c_str());
        DEBUG_PRINT(" sw=%X\n",t);
        if (sw) sw->setMomentary(t);
        DEBUG_PRINT("[Device] Switch %s momentary=%d\n",sw->getId(),t);
    }
    DEBUG_PRINT("[Device] SETUP done\n");

    switchIterator = new ListIterator<GPIOSwitch>(switches);
}

void Device::loop(){
    
    switchIterator->reset();
    while(switchIterator->next()){
        switchIterator->get()->loop();
    }

    sequence.loop();

    if (sequence.running() && millis()-seqStatusUpdatedOn > 1000){
        DEBUG_PRINT("Status sequence %d\n",sequence.running());
        seqStatusUpdatedOn = millis();
        // send sequence processing status 0-100%
        homieDevice.setProperty("seqStatus").send(String(sequence.running()));
    }

    #ifndef NODEBUG_PRINT
    if (millis()-aliveTimer > 15000){
        aliveTimer = millis();
        DEBUG_PRINT("[Device] alive ms=%lu\n",millis());
    }
    #endif
    
}