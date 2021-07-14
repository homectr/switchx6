#include "handlers.h"
#include "Thing.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

extern Thing thing;

bool sequenceHandler(const HomieRange &range, const String &value){
    unsigned long totalDuration = 0;
    unsigned int i = 0;
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
    while(i <= value.length()){
        j = value.indexOf(',',i);
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
            thing.sequence.add(port,duration);
        } else {
            DEBUG_PRINT("Adding pause to sequence: duration=%lu\n",duration);
            thing.sequence.addPause(duration);
        }

        if (j<0) {
            DEBUG_PRINT("break\n");
            break;
        }

        i = j + 1;

    }  

    if (totalDuration > 0) {
        thing.homieDevice.setProperty("seq").send(String(totalDuration));  // send total sequence duraton in ms back
        Homie.getLogger() << "Total sequence duration is " << totalDuration << endl;
        thing.sequence.start();
    }

    return totalDuration > 0;
}


bool cmdHandler(const HomieRange &range, const String &value){
    bool updated = false;

    if (value == "seqence_stop") {
        thing.sequence.stop();
        updated = true;
    }

    if (updated) {
        thing.homieDevice.setProperty("cmd").send(value);  // Update the state of the led
        Homie.getLogger() << "Cmd is " << value << endl;
    }

    return updated;
}


bool updateHandler(const HomieNode &node, const HomieRange &range, const String &property, const String &value){
    DEBUG_PRINT("[updHandler] node=%s prop=%s val=%s\n",node.getId(),property.c_str(),value.c_str());
    bool updated = false;
    String newValue = value;

    if (strcmp(node.getType(),"switch") == 0){
        GPIOSwitch *s = thing.switches.get(property.c_str());
        if (s) {
            newValue = value == "true"?"true":"false";
            if (value == "true") s->on();
            else s->off();
            
            updated = true;
        }
    }

    if (updated) {
        node.setProperty(property).send(newValue);
        Homie.getLogger() << "Node '"<< node.getId() << "' property '" << property << "' set to " << newValue << endl;
    }

    return updated;
}

void handleSequenceStart(){
    thing.homieDevice.setProperty("seqStatus").send("1");
}

void handleSequenceStop(){
    thing.homieDevice.setProperty("seqStatus").send("0");
    thing.sequence.clear();
}

void handleStepStart(SeqStep<unsigned char> *step){
    digitalWrite(step->item,1);
    thing.homieSwitches.setProperty(String(step->item)).send("true");
}

void handleStepStop(SeqStep<unsigned char> *step){
    digitalWrite(step->item,0);
    thing.homieSwitches.setProperty(String(step->item)).send("false");
}

void handleSwitchOn(Switch* s){
    thing.homieSwitches.setProperty(s->getId()).send("true");
}

void handleSwitchOff(Switch* s){
    thing.homieSwitches.setProperty(s->getId()).send("false");
}