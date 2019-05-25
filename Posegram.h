#ifndef Posegram_h
#define Posegram_h

#include <EEPROM.h>
#include "ModeSwitch.h"  // handles 3-position throw switch
#include "Broadcast.h"   // writes PWM output for pose-followers
#include "DutyStruct.h"  // keeps up with the output from another bot's Broadcast
#include "KnobState.h"   // keeps the state of the knobs, X and Y
#include "LightStage.h"  // provides simple interface to NeoPixels, avoids unnecessary writes
#include "MotorState.h"  // writes positions to servos X and Y
#include "PoseData.h"    // handles EEPROM, load on boot and save only when necessary

#define clickDebounce 400
#define defaultTempo 120

class Posegram {
  public:
    Posegram(KnobState *knobState, DutyStruct *X_INPUT, DutyStruct *Y_INPUT, ModeSwitch *modeSwitch, MotorState *motorState, LightStage *lightStage, Broadcast *broadcast);
    void programPosition();
    void playEachPose();
    void followTheLeader();
    byte nextBeat();
  
  private:
    int poseLength;
    byte beat;
    byte lastBeat;
    long lastPose;
    long lastClick;
    Pose temp; // Pose struct loaded from MotorState.h...
    PoseData poses;
    
    KnobState _knobState;
    DutyStruct _X_INPUT;
    DutyStruct _Y_INPUT;
    ModeSwitch _modeSwitch;
    MotorState _motorState;
    LightStage _lightStage;
    Broadcast _broadcast;
};
      
Posegram::Posegram( KnobState *knobState, DutyStruct *X_INPUT, DutyStruct *Y_INPUT, ModeSwitch *modeSwitch, MotorState *motorState, LightStage *lightStage, Broadcast *broadcast){
  _knobState = *knobState;
  _X_INPUT = *X_INPUT;
  _Y_INPUT = *Y_INPUT;
  _modeSwitch = *modeSwitch;
  _motorState = *motorState;
  _lightStage = *lightStage;
  _broadcast = *broadcast;
  beat = 0;
  lastBeat = 7;
  lastPose = millis();
  lastClick = millis();
  poses.loadFromEEPROM();

  // cant decide how to check if valid data array is saved on EEPROM or if we should initialize... 
//  for(int i = 0; i < 8; i++){
//    pose = {120, 255, 120};
//    poses.put(i, pose);
//  }
}

byte Posegram::nextBeat(){
  if(lastClick < millis() - clickDebounce){
      lastClick = millis();
      beat = (beat + 1) % 8;
  }
}

void Posegram::programPosition(){
  _knobState.updateKnobs();
  if(lastBeat != beat){
    lastBeat = beat;
    _knobState.lockKnobs();
    temp = poses.get(beat);
  }
  if(_knobState.stage >= 0){
    temp.stage = _knobState.stage;
    poses.put(beat, temp);
  }
  if(_knobState.backdrop >= 0){
    temp.backdrop = _knobState.backdrop;
    poses.put(beat, temp);
  }
  
  _motorState.updateMotors(temp.backdrop, temp.stage);
  _lightStage.updateBeat(beat, temp.backdrop, temp.stage);
}
void Posegram::playEachPose(){
  poses.saveChanges();
  _knobState.lockKnobs();
  poseLength = 60000 / defaultTempo; 
  
  if(lastPose < (millis() - poseLength)){
    lastPose = millis();
    nextBeat();
    temp = poses.get(beat);
    _lightStage.updateBeat(beat, temp.stage, temp.backdrop);
    _motorState.updateMotors(temp.backdrop, temp.stage);
  }
}

void Posegram::followTheLeader(){}

#endif
