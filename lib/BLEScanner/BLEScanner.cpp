#include "BLEScanner.h"
#include "utils.h"

#include "debug_print.h"
//#define NODEBUG_PRINT

#define ALIVE_INTERVAL 3000

BLEScanner::BLEScanner(const char* id, unsigned char rxPin, unsigned char txPin, unsigned long speed, unsigned char group):Item(id){
    ss = new SoftwareSerial(rxPin,txPin);
    this->speed = speed;
    this->group = group;
}

void BLEScanner::atCommand(const char* cmd){
    String s;
    ss->write(cmd);
    delay(100);
    #ifndef NODEBUG_PRINT
    s = ss->readString();
    DEBUG_PRINT("[BLES] %s=>%s\n",cmd,s.c_str());
    #endif 
}

void BLEScanner::begin(){
    #ifndef NODEBUG_PRINT
    String s;
    #endif

    ss->begin(speed);
    running = 1;

    atCommand("AT");
    atCommand("AT+ROLE1"); // Master mode    
    atCommand("AT+IMME1"); // wait for a connection command before connecting
    atCommand("AT+SCAN9"); // scan for duration of 9 seconds
    atCommand("AT+SHOW1"); // display NAME of discovered device
    atCommand("AT+RESET");

}

unsigned char responseToken[] = "OK+";
#define RESPONSE_TOKEN_SIZE sizeof(responseToken)-1

size_t BLEScanner::shiftBuffer(unsigned char* buff, size_t size, size_t shiftBy){
    if (shiftBy >= size) return 0;
    size = size - shiftBy;
    memcpy(buff,buff+shiftBy,size);
    return size;
}

size_t BLEScanner::processBuffer(unsigned char *buff, size_t size){
    DEBUG_PRINT("[BLES] Buffer=%s\n",buff);
    int r = findBytes(buff, size, responseToken, RESPONSE_TOKEN_SIZE);
    DEBUG_PRINT("[BLES] Token found at %d\n",r);
    if (r >= 0) {
        if (r+8 <= size && memcmp(buff+r, "OK+DISAS", 8) == 0){
            DEBUG_PRINT("[BLES] received start\n");
            size = shiftBuffer(buff,size,r+8);
            return size;
        } else if (r+8 <= size && memcmp(buff+r, "OK+DISCE", 8) == 0){
            DEBUG_PRINT("[BLES] received end\n");
            size = shiftBuffer(buff,size,r+8);
            return size;
        } else if (r+8 <=size && memcmp(buff+r, "OK+DISA:", 8) == 0){
            int ds = r+8;
            int rr = findBytes(buff+ds, size-ds, responseToken, RESPONSE_TOKEN_SIZE);
            if (rr>=0) {
                DISARec rec;
                for(int x=0;x<6;x++) *(rec.mac+x) = *(buffer+ds+5-x); // copy in reverse order
                //memcpy(rec.mac,buffer+ds,6);
                rec.devType = *(buffer+ds+6);
                rec.rssi = *(buffer+ds+6+1);
                rec.dataLength = *(buffer+ds+6+1+1);
                memcpy(rec.data,buffer+ds+6+1+1+1,rec.dataLength);
                DEBUG_PRINT("[BLES] device mac=");
                DEBUG_ARRAY(rec.mac,6);
                DEBUG_PRINT(" type=%02X",rec.devType);
                DEBUG_PRINT(" rssi=%d len=%d data=",(signed char)rec.rssi, rec.dataLength);
                DEBUG_ARRAY(rec.data,rec.dataLength);
                DEBUG_PRINT("\n");
                size = shiftBuffer(buff,size,r+8+rr);
                return size;  
            } else {
                DEBUG_PRINT("[BLES] Expecting more data\n");
            }
            
        }
    } else if (r<0) {
        DEBUG_PRINT("[BLES] token not found\n");
    } else {
        DEBUG_PRINT("[BLES] incomplete data size=%d\n",size);

    }

    return size;
}

void BLEScanner::loop(){
    if (millis()-lastScanStart > BLE_SCAN_INTERVAL){
        ss->write("AT+DISA?");
        lastScanStart = millis();
        DEBUG_PRINT("[BLES] start scan ms=%lu\n", millis());
    }

    if (buffUsed > 0 && lastUsed != buffUsed) {
        DEBUG_PRINT("[BLES] processing buffer %d\n",buffUsed);
        lastUsed = buffUsed;
        buffUsed = processBuffer(buffer, buffUsed);
        DEBUG_PRINT("[BLES] processed buffer %d\n",buffUsed);
    }

    
    if (ss->available()) {
        if (buffUsed < BLE_SCAN_BUFFER_SIZE) {
            int a = ss->readBytes(buffer+buffUsed,BLE_SCAN_BUFFER_SIZE-buffUsed);
            buffUsed = buffUsed + a;
            DEBUG_PRINT("[BLES] received %d bytes\n",a);
        } else {
            DEBUG_PRINT("[BLES] *** Buffer full\n");
        }
    }

    #ifndef NODEBUG_PRINT
    if (millis()-lastAlive > ALIVE_INTERVAL){
        lastAlive = millis();
        DEBUG_PRINT("[BLES] alive ms=%lu\n", millis());
    }
    #endif
}