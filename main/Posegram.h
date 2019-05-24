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
    colorHSV pose;
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
  _motorState.updateMotors(&_knobState);

  if(lastBeat != beat){
    lastBeat = beat;
    _knobState.lockKnobs();
    pose = poses.get(beat);
    _lightStage.updateBeat(beat, pose.h, pose.s, pose.b);
  } else {
    if(_knobState.x >= 0){
      pose.h = map(_knobState.x, 0, 1023, 0, 65536);
      poses.put(beat, pose);
      _lightStage.updateBeat(beat, pose.h, pose.s, pose.b);
    }
    if(_knobState.y >= 0){
      pose.b = map(_knobState.y, 0, 1023, 0, 255);
      poses.put(beat, pose);
      _lightStage.updateBeat(beat, pose.h, pose.s, pose.b);
    }
  }

}
void Posegram::playEachPose(){
  poses.saveChanges();
  _knobState.lockKnobs();
  poseLength = 60000 / defaultTempo; 
  
  if(lastPose < (millis() - poseLength)){
    lastPose = millis();
    nextBeat();
    pose = poses.get(beat);
    _lightStage.updateBeat(beat, pose.h, pose.s, pose.b);
  }
}

void Posegram::followTheLeader(){}

#endif
