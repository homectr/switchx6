#pragma once

#include <Syslog.h>

class Logger {
    protected:
        Syslog* logger = NULL;

    public:
        void setLogger(Syslog* logger){this->logger=logger;};
        
};