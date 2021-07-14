#include "Thing.h"
#include <Arduino.h>
#include "handlers.h"
#include "utils.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

Thing::Thing(){

    for (uint8_t i = 0; i < NUMBER_OF_ITEMS; i++) {
        items[i] = nullptr; // no item so far
        // create configuration setting for each item
        const char *id = strdup(String(i+1).c_str());
        scfg[i] = new HomieSetting<const char*>(id, strdup(String(String("Configuration for item #")+id).c_str()));
    }

    // create properties for device
    homieDevice.advertise("seq").setDatatype("string").settable(sequenceHandler);
    homieDevice.advertise("seqStatus").setDatatype("integer");
    homieDevice.advertise("cmd").setDatatype("string").settable(cmdHandler);

    sequence.handleOnStart(handleSequenceStart);
    sequence.handleOnStop(handleSequenceStop);
    sequence.handleOnStepStart(handleStepStart);
    sequence.handleOnStepStop(handleStepStop);
    DEBUG_PRINT("Thing created\n");
}

Switch* Thing::createSwitch(const char* cfg){
    DEBUG_PRINT("Creating switch from configuraion= %s\n",cfg);

    GPIOSwitch *sw = nullptr;

    char itemId[10];
    if (!getToken(itemId,10,cfg,':',1)) return sw;

    char buff[10];
    if (!getToken(buff,4,cfg,':',2)) return sw;
    int gpio = atoi(buff);
    if (gpio<0) return sw;


    // initialize GPIO
    pinMode(gpio, OUTPUT);
    digitalWrite(gpio, LOW);

    // create switch for GPIO
    const char *id = strdup(itemId);
    sw = new GPIOSwitch(gpio,id);
    sw->setCbOff(handleSwitchOff);
    sw->setCbOn(handleSwitchOn);
    DEBUG_PRINT("Switch created: id=%s gpio=%d", id, gpio);
    
    switches.add(id,sw);

    // if 3rd token is 'm' then it is a momentary switch
    if (getToken(buff,3,cfg,':',3) && strcmp(buff,"m")){
        if (getToken(buff,6,cfg,':',4)){
            long mt = atol(buff);
            if (mt>0) {
                sw->setMomentary(mt);
                DEBUG_PRINT(" momentary=%lu",mt);
            }

        }
    }
    DEBUG_PRINT("\n");

    // configure Homie property for the switch
    homieSwitches.advertise(id).setDatatype("boolean").setRetained(false).settable();       

    return sw;

}

Item* Thing::createItem(const char* cfg){
    DEBUG_PRINT("Creating item using config=%s\n",cfg);

    #define CFG_BUFF_SIZE 50 // size of local buffer
    Item *item = nullptr;

    if (!cfg) return item;
    char itemType[10];
    int tl = getToken(itemType,10,cfg,':',1);
    
    if (tl <= 0) return item;

    if (strcmp(itemType,"switch") == 0){
        return (Item*)createSwitch(cfg+tl+2);
    }

    return item;
}

void Thing::setup(){
    DEBUG_PRINT("[Thing] SETUP\n");

    if (!Homie.isConfigured()){
        CONSOLE("Homie not configured. Skipping Thing setup.\n");
        return;
    }

    for (int i=0; i<NUMBER_OF_ITEMS;i++){
        if (scfg[i]) items[i] = createItem(scfg[i]->get());
    }

    switchIterator = new ListIterator<GPIOSwitch>(switches);
}

void Thing::loop(){
    
    for (int i=0;i<NUMBER_OF_ITEMS;i++){
        DEBUG_PRINT("L=%d",i);
        if (items[i]) items[i]->loop();
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
        DEBUG_PRINT("[Thing] alive ms=%lu\n",millis());
    }
    #endif
    
}