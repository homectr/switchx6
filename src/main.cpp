
#include <Homie.h>
#include "Device.h"
#include "handlers.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

Device device;

void setup() {
    Serial.begin(115200);
    Serial << endl << endl;

    Homie_setFirmware("Switchesx6", "1.0.0");
    Homie.setGlobalInputHandler(updateHandler);
    Homie.setLedPin(15, 1);

    Homie.setup();

    device.setup();
}

unsigned long ms = millis();

void loop() {
    Homie.loop();
    device.loop();
    if (millis()-ms > 5000){
        ms = millis();
        DEBUG_PRINT("[main] alive\n");
    }
    
}