
#include <Homie.h>
#include "Sequence.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

const unsigned int NUMBER_OF_BUTTONS = 6;
const unsigned char GPIOS[NUMBER_OF_BUTTONS] = { 13, 12, 14, 16, 2, 4 };
const String opts = String("on ON 1 off OFF 0");

HomieNode nodeDevice ("device", "Device", "device");
HomieNode nodePorts ("button", "Buttons", "button");

Sequence<unsigned char> sequence("buttons");

bool sequenceHandler(const HomieRange &range, const String &value){
    unsigned long totalDuration = 0;
    int i = 0;
    int j = 0;
    String t;
    unsigned int multiple = 1;
    
    DEBUG_PRINT("handling new sequence=%s\n",value.c_str());

    // parse program: time:IO:duration:IO:duration....
    // example s:1:10: 2:15:

    j = value.indexOf(':',i);
    if (j<0) return false;

    t = value.substring(i,j);
    DEBUG_PRINT("time specified in %s\n",t.c_str());
    if (t=="ms") multiple = 1;
    if (t=="s") multiple = 1000;
    if (t=="m") multiple = 60000;
    if (t=="h") multiple = 3600000;

    i=j+1;
    while(i<=value.length()){
        j = value.indexOf(':',i);
        if (j<0) {
            DEBUG_PRINT("break\n");
            break;
        }
        DEBUG_PRINT("Found i=%d j=%d end port %s\n",i,j,value.substring(i,j).c_str());
        unsigned char port = value.substring(i,j).toInt();
        i = j + 1;

        j = value.indexOf(':',i);
        DEBUG_PRINT("Found i=%d j=%d end dur %s\n",i,j,value.substring(i,j).c_str());
        unsigned long duration = value.substring(i,j).toInt() * multiple;
        totalDuration += duration;

        if (port>0){
            DEBUG_PRINT("Adding to sequence: port=%d duration=%lu\n",port,duration);
            sequence.add(port,duration);
        } else {
            DEBUG_PRINT("Adding pause to sequence: duration=%lu\n",duration);
            sequence.addPause(duration);
        }

        if (j<0) {
            DEBUG_PRINT("break\n");
            break;
        }

        i = j + 1;

    }  

    if (totalDuration > 0) {
        nodeDevice.setProperty("seq").send(String(totalDuration));  // send total sequence duraton in ms back
        Homie.getLogger() << "Total sequence duration is " << totalDuration << endl;
        sequence.start();
    }

    return totalDuration > 0;
}


bool cmdHandler(const HomieRange &range, const String &value){
    bool updated = false;

    if (value == "seqence_stop") {
        sequence.stop();
        updated = true;
    }

    if (updated) {
        nodeDevice.setProperty("cmd").send(value);  // Update the state of the led
        Homie.getLogger() << "Cmd is " << value << endl;
    }

    return updated;
}


bool updateHandler(const HomieNode &node, const HomieRange &range, const String &property, const String &value){
    bool updated = false;
    String newValue = value;

    if (strcmp(node.getId(),"io") == 0){
        unsigned char port = property.charAt(0)-49;
        newValue = value == "true"?"true":"false";
        digitalWrite(GPIOS[port],value == "true");
        updated = true;
    }

    if (updated) {
        node.setProperty(property).send(newValue);
        Homie.getLogger() << "Property " << property << " set to " << newValue << endl;
    }

    return updated;
}

void handleSequenceStart(){
    nodeDevice.setProperty("seqStatus").send("1");
}

void handleSequenceStop(){
    nodeDevice.setProperty("seqStatus").send("0");
    sequence.clear();
}

void handleStepStart(SeqStep<unsigned char> *step){
    digitalWrite(step->item,1);
    nodePorts.setProperty(String(step->item)).send("true");
}

void handleStepStop(SeqStep<unsigned char> *step){
    digitalWrite(step->item,0);
    nodePorts.setProperty(String(step->item)).send("false");
}


void setup() {
    for (uint8_t i = 0; i < NUMBER_OF_BUTTONS; i++) {
        pinMode(GPIOS[i], OUTPUT);
        digitalWrite(GPIOS[i], LOW);
    }

    Serial.begin(115200);
    Serial << endl << endl;

    Homie_setFirmware("IOCtrl", "1.0.0");
    Homie.setGlobalInputHandler(updateHandler);
    Homie.setLedPin(15, 1);  

    nodeDevice.advertise("seq").setDatatype("string").settable(sequenceHandler);
    nodeDevice.advertise("seqStatus").setDatatype("integer");
    nodeDevice.advertise("cmd").setDatatype("string").settable(cmdHandler);

    nodePorts.advertise("1").setDatatype("boolean").settable();
    nodePorts.advertise("2").setDatatype("boolean").settable();
    nodePorts.advertise("3").setDatatype("boolean").settable();
    nodePorts.advertise("4").setDatatype("boolean").settable();
    nodePorts.advertise("5").setDatatype("boolean").settable();
    nodePorts.advertise("6").setDatatype("boolean").settable();

    Homie.setup();

    sequence.handleOnStart(handleSequenceStart);
    sequence.handleOnStop(handleSequenceStop);
    sequence.handleOnStepStart(handleStepStart);
    sequence.handleOnStepStop(handleStepStop);
}

unsigned long ms = millis();

void loop() {
    Homie.loop();
    sequence.loop();
    if (sequence.running() && millis()-ms > 1000){
        DEBUG_PRINT("Status sequence %d\n",sequence.running());
        ms = millis();
        // send sequence processing status 0-100%
        nodeDevice.setProperty("seqStatus").send(String(sequence.running()));
    }
}