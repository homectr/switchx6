#pragma once

class Item {
    protected:
        const char* id = NULL;
    public:
        virtual void loop(){};
        Item(const char* id){this->id=id;}
        const char* getId(){return id;};
};