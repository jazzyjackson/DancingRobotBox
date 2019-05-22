#ifndef Posegram_h
#define Posegram_h

#include "ModeSwitch.h"  // handles 3-position throw switch
#include "Broadcast.h"   // writes PWM output for pose-followers
#include "DutyStruct.h"  // keeps up with the output from another bot's Broadcast
#include "KnobState.h"   // keeps the state of the knobs, X and Y
#include "LightStage.h"  // provides simple interface to NeoPixels, avoids unnecessary writes
#include "MotorState.h"  // writes positions to servos X and Y

#define debounceTimeout 200
#define defaultTempo 120

class Posegram {
  public:
    Posegram(KnobState* knobState, DutyStruct* X_INPUT, DutyStruct* Y_INPUT, ModeSwitch* modeSwitch, MotorState* motorState, LightStage* lightStage, Broadcast* broadcast);
    void programPosition();
    void playEachPose();
    void followTheLeader();
  
  private:
    int poseLength;
    byte beat;
    byte lastBeat;
    long lastPose;
    long debounceNext;
      
    KnobState _knobState;
    DutyStruct _X_INPUT;
    DutyStruct _Y_INPUT;
    ModeSwitch _modeSwitch;
    MotorState _motorState;
    LightStage _lightStage;
    Broadcast _broadcast;
};
      
Posegram::Posegram(
    KnobState* knobState,
    DutyStruct* X_INPUT,
    DutyStruct* Y_INPUT,
    ModeSwitch* modeSwitch,
    MotorState* motorState,
    LightStage* lightStage,
    Broadcast* broadcast
){
  _knobState = *knobState;
  _X_INPUT = *X_INPUT,
  _Y_INPUT = *Y_INPUT,
  _modeSwitch = *modeSwitch,
  _motorState = *motorState,
  _lightStage = *lightStage,
  _broadcast = *broadcast
}
      
void Posegram::nextBeat(){
  if(debounceNext < millis() - debounceTimeout){
    debounceNext = millis();
    beat = (beat + 1) % 8;
  }
}
      
void Posegram::programPosition(){
}

void Posegram::playEachPose(){
  // check what the last update to X_INPUT/ Y_INPUT was....
  poseLength = 60000 / defaultTempo;
 
  if(lastPose < millis() - poseLength){
    lastPose = millis();
  }
}

void Posegram::followTheLeader(){
}

#endif
