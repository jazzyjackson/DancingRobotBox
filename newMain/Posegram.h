#ifndef Posegram_h
#define Posegram_h

#include <EEPROM.h>
#include "ModeSwitch.h"  // handles 3-position throw switch
#include "KnobState.h"   // keeps the state of the knobs, X and Y
#include "LightStage.h"  // provides simple interface to NeoPixels, avoids unnecessary writes
#include "MotorState.h"  // writes positions to servos X and Y
#include "PoseData.h"    // handles EEPROM, load on boot and save only when necessary
#include "PoseStruct.h"
#include "debug.h"

#define clickDebounce 400
#define defaultTempo 120



class Posegram {
  public:
    Posegram(KnobState *knobState, ModeSwitch *modeSwitch, MotorState *motorState, LightStage *lightStage);
    void programPosition();
    void playEachPose();
    void followTheLeader();
    byte nextBeat();
    void init();
  
  private:
    int poseLength;
    byte beat;
    byte lastBeat;
    long lastPose;
    long lastClick;
    void initPoses(int initValue);
    Pose temp; // Pose struct loaded from MotorState.h...
    PoseData poses;
    
    KnobState _knobState;
    ModeSwitch _modeSwitch;
    MotorState _motorState;
    LightStage _lightStage;
};
      
Posegram::Posegram( KnobState *knobState, ModeSwitch *modeSwitch, MotorState *motorState, LightStage *lightStage){
  _knobState = *knobState;
  _modeSwitch = *modeSwitch;
  _motorState = *motorState;
  _lightStage = *lightStage;
  beat = 0;
  lastBeat = 7;
  lastPose = millis();
  lastClick = millis();
}

void Posegram::init(){
    debug("Posegram is initializing");
    _motorState.init();
    _lightStage.init();
    poses.loadFromEEPROM();
    temp = poses.get(0);
    if(temp.stage == -1 or temp.backdrop == -1){
      initPoses(512); // half way between 0 and 1023, middle knob position
    }
}

void Posegram::initPoses(int initValue){
  temp.stage = initValue;
  temp.backdrop = initValue;
  for(int i = 0; i < 8; i++){
    poses.put(i, temp);
  }
  poses.saveChanges();
}

/** nextBeat
 *  called from ISR
 */
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
  
  _motorState.updateMotors(temp.stage, temp.backdrop);
  _lightStage.updateBeat(beat, temp.stage, temp.backdrop);
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
    _motorState.updateMotors(temp.stage, temp.backdrop);
  }
}

void Posegram::followTheLeader(){}

#endif
