#ifndef Posegram_h
#define Posegram_h

#include "ModeSwitch.h"  // handles 3-position throw switch
#include "Broadcast.h"   // writes PWM output for pose-followers
#include "DutyStruct.h"  // keeps up with the output from another bot's Broadcast
#include "KnobState.h"   // keeps the state of the knobs, X and Y
#include "LightStage.h"  // provides simple interface to NeoPixels, avoids unnecessary writes
#include "MotorState.h"  // writes positions to servos X and Y

#define clickDebounce 400
#define defaultTempo 120

typedef struct {
  int h;
  int s;
  int b;
} colorHSV;


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
    colorHSV poses[8];
    
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

  for(int i = 0; i < 8; i++){
    poses[i] = {120, 255, 120};
  }
}

byte Posegram::nextBeat(){
  if(lastClick < millis() - clickDebounce){
      lastClick = millis();
      beat = (beat + 1) % 8;
  }
}

void Posegram::programPosition(){
  
  _knobState.updateKnobs();
 // _motorState.updateMotor(&_knobState)

  if(lastBeat != beat){
    lastBeat = beat;
    _knobState.lockKnobs();
    _lightStage.updateBeat(beat, poses[beat].h, poses[beat].s, poses[beat].b);
  } else {
    if(_knobState.x >= 0){
      // overwrite the stored hue only if knobs are unlocked (-1 is locked, >= 0 is unlocked)
      poses[beat].h = map(_knobState.x, 0, 1023, 0, 65536);
      _lightStage.updateBeat(beat, poses[beat].h, poses[beat].s, poses[beat].b);
    }
    if(_knobState.y >= 0){
      poses[beat].b = map(_knobState.y, 0, 1023, 0, 255);
      _lightStage.updateBeat(beat, poses[beat].h, poses[beat].s, poses[beat].b);
    }
  }
 
//  EEPROM.get(beat, pose)
  // check if we're on a new beat
  // if so, load from memory (updateBeat),
    // set 'knobLocked'
    // store knob position
    // exit

  // if knob locked, don't worry about it
}

void Posegram::playEachPose(){
  // check what the last update to X_INPUT/ Y_INPUT was....
  poseLength = 60000 / defaultTempo;
  
  if(lastPose < (millis() - poseLength)){
    lastPose = millis();
    nextBeat();
    _lightStage.updateBeat(beat, 255, 255, 255);
  }
}

void Posegram::followTheLeader(){
}

#endif