#include "Thing.h"
#include <Arduino.h>
#include "handlers.h"
#include "utils.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

Thing::Thing(){

    for (uint8_t i = 0; i < NUMBER_OF_ITEMS; i++) {
        items[i] = NULL; // no item so far
        // create configuration setting for each item
        const char *id = strdup(String(String("item")+(i+1)).c_str());
        const char *desc = strdup(String(String("Configuration for item #")+id).c_str());
        itemCfg[i] = new HomieSetting<const char*>(id, desc);
        DEBUG_PRINT("[Thing:Thing] Creating config item: id=%s desc=%s\n",id, desc);
    }

    // create properties for device
    homieDevice.advertise("cmd").setDatatype("string").settable(cmdHandler);
    homieDevice.advertise("seq").setDatatype("string").settable(sequenceHandler);
    homieDevice.advertise("seqStatus").setDatatype("integer");

    /*
    sequence.handleOnStart(handleSequenceStart);
    sequence.handleOnStop(handleSequenceStop);
    sequence.handleOnStepStart(handleStepStart);
    sequence.handleOnStepStop(handleStepStop);
    */
    DEBUG_PRINT("[Thing:Thing] Thing created\n");
}

Switch* Thing::createSwitch(const char* cfgDelim){
    GPIOSwitch *sw = NULL;

    const char* id = strtok(NULL, cfgDelim);
    const char* gpioStr = strtok(NULL, cfgDelim);

    if (!id || !gpioStr) return sw;

    int gpio = atoi(gpioStr);
    if (gpio < 0) return sw;

    // initialize GPIO
    pinMode(gpio, OUTPUT);
    digitalWrite(gpio, LOW);

    // create switch for GPIO
    sw = new GPIOSwitch(gpio,strdup(id));
    sw->setCbOff(handleSwitchOff);
    sw->setCbOn(handleSwitchOn);
    DEBUG_PRINT("Switch created: id=%s gpio=%d", id, gpio);
    
    switches.add(id,sw);

    // if 3rd token is 'm' then it is a momentary switch
    const char* momt = strtok(NULL,cfgDelim);
    if (strcmp(momt,"m")){
        const char* mtStr = strtok(NULL,cfgDelim);
        if (mtStr){
            long mt = atol(mtStr);
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
    const char* cfgDelim=":";

    #define CFG_BUFF_SIZE 50 // size of local buffer
    Item *item = NULL;

    if (!cfg) return item;

    // create modifiable config string
    char cb[CFG_BUFF_SIZE];
    strcpy(cb,cfg);

    char *tok = strtok(cb,cfgDelim);
    
    if (!tok || *tok==0) return item;

    if (strcmp(tok,"switch") == 0){
        return (Item*)createSwitch(cfgDelim);
    }

    return item;
}

void Thing::setup(){
    DEBUG_PRINT("[Thing:Setup] SETUP\n");

    if (!Homie.isConfigured()){
        CONSOLE("Homie not configured. Skipping Thing setup. Loop will be ignored.\n");
        return;
    }

    DEBUG_PRINT("[Thing:Setup] Creating items\n");

    for (int i=0; i<NUMBER_OF_ITEMS;i++){
        if (itemCfg[i]) items[i] = createItem(itemCfg[i]->get());
    }

    switchIterator = new ListIterator<GPIOSwitch>(switches);

    DEBUG_PRINT("[Thing:Setup] Completed\n");

    configured = true;
}

void Thing::loop(){
    if (!isConfigured()) return;
    DEBUG_PRINT(" TL ");
    for (int i=0;i<NUMBER_OF_ITEMS;i++){
        if (items[i]) {
            DEBUG_PRINT(" ITL=%d",i);
            items[i]->loop();
        }
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