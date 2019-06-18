#ifndef PoseData_h
#define PoseData_h

#define NUM_POSES 8
#include <EEPROM.h>

#include "debug.h"
#include "PoseStruct.h"

class PoseData {
  private:
    int      address;
    long     lastModified;
    long     lastSave;
    Pose temp;
    Pose data[NUM_POSES]; // array of poses

  public:
    PoseData();
    void     saveChanges();
    void     loadFromEEPROM();
    void     saveToEEPROM();
    Pose     get(byte beatNumber);
    void     put(byte beatNumber, Pose pose);
};

PoseData::PoseData(){}

void PoseData::saveChanges(){
    if(lastModified > lastSave){
      saveToEEPROM();
    }
}
  
void PoseData::loadFromEEPROM(){
  for(int i = 0; i < 8; i++){
    address = i * sizeof(Pose);
    EEPROM.get(address, temp);
    data[i] = temp;
  }
  debug("EEPROM: load:", data, NUM_POSES);
}

void PoseData::saveToEEPROM(){
  lastSave = millis();
  for(int i = 0; i < 8; i++){
    address = i * sizeof(Pose);
    EEPROM.put(address, data[i]);
  }
  debug("EEPROM: saved:", data, NUM_POSES);
}

Pose PoseData::get(byte index){
  return data[index];
}

void PoseData::put(byte index, Pose pose){
  lastModified = millis();
  data[index] = pose;
}

#endif
