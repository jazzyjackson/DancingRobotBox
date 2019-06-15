#ifndef PoseData_h
#define PoseData_h

typedef struct {
  short backdrop;
  short stage;
} Pose;

class PoseData {
  private:
    int      address;
    long     lastModified;
    long     lastSave;
    Pose temp;    // Pose struct loaded from MotorState.h is {short stage, short backdrop}
    Pose data[8]; // array of poses

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
  for(int i; i<8; i++){
    address = i * sizeof(Pose);
    EEPROM.get(address, temp);
    data[i] = temp;
  }
}

void PoseData::saveToEEPROM(){
  lastSave = millis();
  for(int i; i<8; i++){
    address = i * sizeof(Pose);
    EEPROM.put(address, data[i]);
  }
}

Pose PoseData::get(byte index){
  return data[index];
}

void PoseData::put(byte index, Pose pose){
  lastModified = millis();
  data[index] = pose;
}

#endif
