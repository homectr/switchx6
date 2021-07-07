#pragma once

#include <Arduino.h>

//#define NODEBUG_PRINT
#include "debug_print.h"

template <class T>
struct SeqStep {
    unsigned int id;
    unsigned char pause;
    T item;
    unsigned long duration;
    SeqStep* next;
};

template <class T>
class Sequence {
    using stepEvent_cb_t = void(*)(SeqStep<T>* item);
    using event_cb_t = void(*)();
    protected:
        const char *id;

        SeqStep<T>* sequence = nullptr;
        unsigned long sequenceDuration = 0;
        unsigned int sequenceLength = 0;

        unsigned char isRunning = 0;
        unsigned long executedDuration = 0;
        unsigned long runningDuration = 0;
        SeqStep<T>* pos = nullptr; // current position in the started sequence
        unsigned long posStartedAt;

        event_cb_t onStart = nullptr;
        event_cb_t onStop = nullptr;
        stepEvent_cb_t onStepStart = nullptr;
        stepEvent_cb_t onStepStop = nullptr;

        void _add(SeqStep<T> *step);

    public:
        Sequence(const char *id);
        void add(T item, unsigned long duration);
        void addPause(unsigned long duration);
        void start();
        virtual void start(SeqStep<T> *item);
        virtual void stop(SeqStep<T> *item);
        void stop();
        void clear();
        void loop();
        unsigned int length(){return sequenceLength;};
        unsigned int duration(){return sequenceDuration;};
        unsigned char running();

        void handleOnStart(event_cb_t onStart){this->onStart = onStart;};
        void handleOnStop(event_cb_t onStop){this->onStop = onStop;};
        void handleOnStepStop(stepEvent_cb_t onStop){this->onStepStop = onStop;};
        void handleOnStepStart(stepEvent_cb_t onStart){this->onStepStart = onStart;};
    
};

template <class T>
Sequence<T>::Sequence(const char* id){
    this->id = id;
}

template <class T>
void Sequence<T>::_add(SeqStep<T> *step){
    if (sequence) {
        SeqStep<T> *i = sequence;
        while (i->next) i = i->next;
        i->next = step;
    } else {
        sequence = step;
    }

    sequenceDuration += step->duration;
    sequenceLength++;

    DEBUG_PRINT("[Sequence %s] Added step %d\n", id, step->id);
}


template <class T>
void Sequence<T>::add(T item, unsigned long duration){
    SeqStep<T> *s = new SeqStep<T>();
    s->pause = 0;
    s->duration = duration;
    s->item = item;
    s->next = nullptr;
    s->id = sequenceLength + 1;

    _add(s);
}

template <class T>
void Sequence<T>::addPause(unsigned long duration){
    SeqStep<T> *s = new SeqStep<T>();
    s->pause = 1;
    s->duration = duration;
    s->next = nullptr;
    s->id = sequenceLength + 1;

    _add(s);
}


template <class T>
void Sequence<T>::clear(){
    SeqStep<T> *i = sequence;
    while (i) {
        SeqStep<T> *j = i;
        i=i->next;
        delete j;
    }
    sequence = nullptr;
    sequenceDuration = 0;
    sequenceLength = 0;

    DEBUG_PRINT("[Sequence %s] Cleared\n",id);
}

template <class T>
void Sequence<T>::start(){
    pos = sequence;
    isRunning = 1;
    executedDuration = 0;
    start(pos);
    if (onStart) this->onStart();
    DEBUG_PRINT("[Sequence %s] Started\n",id);
}

template <class T>
void Sequence<T>::stop(SeqStep<T> *pos){
    if (!pos) return;
    if (!pos->pause && onStepStop) onStepStop(pos);
    DEBUG_PRINT("[Sequence %s] Stopped step %d\n",id, pos->id);
}


template <class T>
void Sequence<T>::stop(){
    isRunning = 0;
    stop(pos);
    if (onStop) onStop();
    DEBUG_PRINT("[Sequence %s] Stopped.\n",id);
}

template <class T>
void Sequence<T>::start(SeqStep<T> *pos){
    if (!pos) return;
    DEBUG_PRINT("[Sequence %s] starting step %d\n",id, pos->id);

    this->pos = pos;
    this->posStartedAt = millis();

    if (!pos->pause && onStepStart) onStepStart(pos);
}

template <class T>
void Sequence<T>::loop(){
    if (!isRunning) return;

    runningDuration = millis()-posStartedAt;
    if ( runningDuration > pos->duration){
        executedDuration += pos->duration;
        stop(pos);
        if (pos->next) start(pos->next);
        else stop();
    }

}

template <class T>
unsigned char Sequence<T>::running(){
    if (!isRunning) return 0;
    return (executedDuration+runningDuration)*100/sequenceDuration;
}
