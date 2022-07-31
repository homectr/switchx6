
#include <Homie.h>
#include <Syslog.h>
#include "Thing.h"
#include "handlers.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

#define HOMIE_LED_PIN 2
#define FIRMWARE_NAME "sxpwm"
#define FIRMWARE_VERSION "1.0.0"

#define PWM_FRWQUENCY 1000 // in Hz
#define PWM_RANGE 100

#define ALIVE_TIMER 15000 // used to print "alive" messages in debug mode

Thing *thing = NULL;

HomieNode homieDevice = HomieNode("device", "Device", "device");
HomieSetting<const char *> syslogSrv("syslogsrv", "Syslog server hostname/ip");
HomieSetting<long> syslogPort("syslogport", "Syslog port");

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udpClient;

// Create a new empty syslog instance
Syslog syslog(udpClient, SYSLOG_PROTO_IETF);

void setup()
{
    Serial.begin(115200);
    Serial << endl
           << endl;

    Homie_setFirmware(FIRMWARE_NAME, FIRMWARE_VERSION);
    Homie.setGlobalInputHandler(updateHandler);
    Homie.setLedPin(HOMIE_LED_PIN, 1);

    syslogSrv.setDefaultValue("");
    syslogPort.setDefaultValue(514).setValidator([](long p)
                                                 { return (p >= 0) && (p <= 0xFFFF); });

    thing = new Thing();

    Homie.setup();
    thing->setup(); // call device setup only after Homie setup has been called

    analogWriteFreq(PWM_FRWQUENCY); // set 1000Hz frequency for PWM
    analogWriteRange(PWM_RANGE);    // set pwm range to 0-100 (for easy calculation)
}

unsigned long ms = millis();

void loop()
{
    Homie.loop();
    Homie.onEvent(onHomieEvent);
    thing->loop();
#ifndef NODEBUG_PRINT
    if (millis() - ms > ALIVE_TIMER)
    {
        ms = millis();
        DEBUG_PRINT("[main] alive ms=%lu\n", millis());
    }
#endif
}