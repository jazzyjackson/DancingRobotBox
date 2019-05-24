#include "MotorState.h" // includes definition for colorHSV, soon to be MotorState.pose...

class PoseData {
  private:
    int      address;
    long     lastModified;
    long     lastSave;
    colorHSV temp;
    colorHSV data[8];

  public:
    PoseData();
    void     saveChanges();
    void     loadFromEEPROM();
    void     saveToEEPROM();
    colorHSV get(byte beatNumber);
    void     put(byte beatNumber, colorHSV color);
    
};

PoseData::PoseData(){}

void PoseData::saveChanges(){
    if(lastModified > lastSave){
      lastSave = millis();
      saveToEEPROM();
    }
}
  
void PoseData::loadFromEEPROM(){
  for(int i; i<8; i++){
    address = i * sizeof(colorHSV);
    EEPROM.get(address, temp);
    data[i] = temp;
  }
}

void PoseData::saveToEEPROM(){
  for(int i; i<8; i++){
    address = i * sizeof(colorHSV);
    EEPROM.put(address, data[i]);
  }
}

colorHSV PoseData::get(byte index){
  return data[index];
}

void PoseData::put(byte index, colorHSV color){
  lastModified = millis();
  data[index] = color;
}
