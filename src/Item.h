#pragma once
#include <Syslog.h>
#include "Logger.h"

class Item: public Logger {
    protected:
        const char* id;
        
    public:
        Item(const char* id){this->id=id;}
        virtual void loop(){};
        const char* getId(){return id;};
};