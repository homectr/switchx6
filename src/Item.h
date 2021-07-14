#pragma once

class Item {
    public:
        const char* id = nullptr;
        virtual void loop()=0;
        Item(const char* id){this->id=id;}
        const char* getId(){return id;};
};