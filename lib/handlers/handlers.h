#pragma once

#include <Homie.h>
#include "Sequence.h"
#include "Switch.h"
#include "PWMPort.h"

bool sequenceHandler(const HomieRange &range, const String &value);
bool cmdHandler(const HomieRange &range, const String &value);
bool updateHandler(const HomieNode &node, const HomieRange &range, const String &property, const String &value);

void handleSequenceStart();
void handleSequenceStop();

void handleStepStart(SeqStep<unsigned char> *step);
void handleStepStop(SeqStep<unsigned char> *step);

void handleSwitchOn(Switch* s);
void handleSwitchOff(Switch* s);

void handlePWM(PWMPort* p);