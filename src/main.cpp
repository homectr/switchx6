
#include <Homie.h>
#include "Thing.h"
#include "handlers.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

#define HOMIE_LED_PIN       2
#define FIRMWARE_NAME       "sxpwm"
#define FIRMWARE_VERSION    "1.0.0"

#define PWM_FRWQUENCY       1000  // in Hz
#define PWM_RANGE           100   

#define ALIVE_TIMER         15000  // used to print "alive" messages in debug mode

Thing* thing = NULL;

HomieNode homieDevice = HomieNode("device", "Device", "device");

void setup() {
    Serial.begin(115200);
    Serial << endl << endl;

    Homie_setFirmware(FIRMWARE_NAME, FIRMWARE_VERSION);
    Homie.setGlobalInputHandler(updateHandler);
    Homie.setLedPin(HOMIE_LED_PIN, 1);

    thing = new Thing();

    Homie.setup();
    thing->setup(); // call device setup only after Homie setup has been called

    analogWriteFreq(PWM_FRWQUENCY); // set 1000Hz frequency for PWM
    analogWriteRange(PWM_RANGE); // set pwm range to 0-100 (for easy calculation)
}

unsigned long ms = millis();

void loop() {
    Homie.loop();
    thing->loop();
    #ifndef NODEBUG_PRINT
    if (millis()-ms > ALIVE_TIMER){
        ms = millis();
        DEBUG_PRINT("[main] alive ms=%lu\n",millis());
    }
    #endif
    
}