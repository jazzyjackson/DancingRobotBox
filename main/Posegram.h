/*
 * Posegram.h includes most of the coordinating logic for DancingRobotBox
 * After copying all the classes passed by reference into private variables,
 * Posegram.init() is called from the setup(){} function in main.ino,
 * which then calls MotorState.init() to perform servo.attach(),
 * and then LightStage.init() to perform calls Adafruit strip.begin() and strip.clear()
 *
 * Also within init() we load 8 'Pose' structs from EEPROM,
 * if the first memory location is '-1', this means nothing has ever been saved to EEPROM,
 * and all 8 EEPROM locations are overwritten with {stage: 512, backdrop: 512} data structs,
 * the center position as a good starting point. (All position values are 10bit, 0 - 1023)
 *
 * Every time around the loop(){} calls 1 of programPosition(), makeTempo(), or playEachPose()
 *
 * These functions rely on the posegram.nextBeat() ISR to update the 'beat' value, which represents
 * the current beat to be programs, played, or measured for tempo.
 *
 * lastBeat and lastPose are checked before taking certain actions,
 * if current beat == lastBeat, certain actions can be skipped / noop'd
 *
 */


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
    void makeTempo();
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

void Posegram::makeTempo(){}

#endif
