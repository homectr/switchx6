
#include <Homie.h>
#include "Thing.h"
#include "handlers.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

Thing thing;

void setup() {
    Serial.begin(115200);
    Serial << endl << endl;

    Homie_setFirmware("Thing", "1.0.0");
    Homie.setGlobalInputHandler(updateHandler);
    Homie.setLedPin(15, 1);

    Homie.setup();
    thing.setup(); // call device setup only after Homie setup has been called

    analogWriteFreq(1000); // set 1000Hz frequency for PWM
    analogWriteRange(100); // set pwm range to 0-100 (for easy calculation)
}

unsigned long ms = millis();

void loop() {
    Homie.loop();
    thing.loop();
    #ifndef NODEBUG_PRINT
    if (millis()-ms > 5000){
        ms = millis();
        DEBUG_PRINT("[main] alive\n");
    }
    #endif
    
}