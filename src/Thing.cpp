#include "Thing.h"
#include <Arduino.h>
#include "handlers.h"
#include "utils.h"

#define NODEBUG_PRINT
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

    sequence.handleOnStart(handleSequenceStart);
    sequence.handleOnStop(handleSequenceStop);
    sequence.handleOnStepStart(handleStepStart);
    sequence.handleOnStepStop(handleStepStop);
    
    DEBUG_PRINT("[Thing:Thing] Thing created\n");
    if (this->logger) logger->logf_P(LOG_INFO,PSTR("Thing: created: items=%d"),NUMBER_OF_ITEMS);
}

Switch* Thing::createSwitch(const char* cfgDelim){
    GPIOSwitch *sw = NULL;

    const char* id = strtok(NULL, cfgDelim);
    const char* gpioStr = strtok(NULL, cfgDelim);
    bool inverse = false;
    long mt = 0;
    bool isOn = false;

    if (!id || !gpioStr) return sw;

    int gpio = atoi(gpioStr);
    if (gpio < 0) return sw;

    DEBUG_PRINT("Switch: id=%s gpio=%d", id, gpio);
    char* tok = strtok(NULL, cfgDelim);
    while (tok){
        if (strcmp(tok,"inv") == 0){
            inverse = true;
            DEBUG_PRINT(" inverse");
        }

        if (strcmp(tok,"m") == 0){
            const char* mtStr = strtok(NULL,cfgDelim);
            if (mtStr) mt = atol(mtStr);
            else mt=10;
            if (mt>0) DEBUG_PRINT(" momentary=%lu",mt);               
        }

        if (strcmp(tok,"ison") == 0){
            isOn = true;
            DEBUG_PRINT(" is=ON");               
        }

        DEBUG_PRINT("\n");
        tok = strtok(NULL, cfgDelim);
    }

    // create switch for GPIO
    sw = new GPIOSwitch(gpio,strdup(id),inverse);
    sw->setCbOff(handleSwitchOff);
    sw->setCbOn(handleSwitchOn);
    if (mt) sw->setMomentary(mt);
    if (isOn) sw->on();
    else sw->off();
        
    switches.add(sw->getId(),sw);
    
    DEBUG_PRINT("... created\n");
    if (this->logger) logger->logf_P(LOG_INFO,PSTR("Thing: switch created: id=%s gpio=%d momentary=%d ison=%d"),id,gpio,mt,isOn);

    // configure Homie property for the switch
    homieSwitches.advertise(id).setDatatype("boolean").setRetained(true).settable();       

    return sw;

}

PWMPort* Thing::createPWM(const char* cfgDelim){
    PWMPort *sw = NULL;

    const char* id = strtok(NULL, cfgDelim);
    const char* gpioStr = strtok(NULL, cfgDelim);

    if (!id || !gpioStr) return sw;

    int gpio = atoi(gpioStr);
    if (gpio < 0) return sw;

    // create switch for GPIO
    sw = new PWMPort(strdup(id), gpio);
    sw->setCbOnChange(handlePWM);

    DEBUG_PRINT("PWM created: id=%s gpio=%d", id, gpio);
    
    pwm.add(sw->getId(),sw);

    const char* dcStr = strtok(NULL,cfgDelim);
    if (dcStr){
        int dc = atoi(dcStr);
        if (dc>0 && dc <=100) {
            sw->setDutyCycle(dc);
            DEBUG_PRINT(" duty=%lu",dc);
        }        
    }
    DEBUG_PRINT("\n");
    if (this->logger) logger->logf_P(LOG_INFO,PSTR("Thing: pwm created: id=%s gpio=%d"),id,gpio);

    // configure Homie property for the switch
    homiePWM.advertise(id).setDatatype("integer").setFormat("0:100").setRetained(true).settable();       

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

    if (strcmp(tok,"pwm") == 0){
        return (Item*)createPWM(cfgDelim);
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

    DEBUG_PRINT("[Thing:Setup] Completed\n");

    configured = true;
}

void Thing::loop(){
    #ifndef NODEBUG_PRINT
    if (millis()-aliveTimer > 15000){
        aliveTimer = millis();
        DEBUG_PRINT("[Thing] alive ms=%lu\n",millis());
        if (!isConfigured()) 
            DEBUG_PRINT("[Thing] not configured. Skipping loop.\n");
    }
    #endif

    if (!isConfigured()) return;
    for (int i=0;i<NUMBER_OF_ITEMS;i++){
        if (items[i]) items[i]->loop();        
    }

    sequence.loop();

    if (sequence.running() && millis()-seqStatusUpdatedOn > 1000){
        DEBUG_PRINT("Status sequence %d\n",sequence.running());
        seqStatusUpdatedOn = millis();
        // send sequence processing status 0-100%
        homieDevice.setProperty("seqStatus").send(String(sequence.running()));
    }
    
}