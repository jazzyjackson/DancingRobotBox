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
 * TODO don't make copies of everything. Use arrow notation to access methods.
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

// debounce time between clicks is .1 second, or 100 milliseconds, or 100000 microseconds
// tempo 120 bpm = 60 / 120 = 0.5s per cycle, or 500 milliseconds, or 500000 microseconds
#define CLICK_DEBOUNCE       200000
#define DEFAULT_POSE_LENGTH  500000
#define NUM_BEAT_INTERVALS   3

#define five  4
#define six   5
#define seven 6
#define eight 7

class Posegram {
  public:
    Posegram(KnobState *knobState, ModeSwitch *modeSwitch, MotorState *motorState, LightStage *lightStage);
    void programPosition();
    void playEachPose();
    void makeTempo();
    byte nextBeat();
    void init();
  
  private:
    void playThisPose();
    void initPoses(int initValue);
    
    volatile byte beat;
    volatile unsigned long  lastInterval; 
    volatile unsigned long  lastClick;
    
    unsigned long beatIntervals[NUM_BEAT_INTERVALS];
    unsigned long lastPose;
    unsigned long poseLength;
    
    bool tempoModified;
    byte lastBeat;
    
    Pose temp; // Pose struct loaded from MotorState.h...
    PoseData poses;
    
    KnobState _knobState;
    ModeSwitch _modeSwitch;
    MotorState _motorState;
    LightStage _lightStage;
};
      
Posegram::Posegram( KnobState *knobState, ModeSwitch *modeSwitch, MotorState *motorState, LightStage *lightStage){
  _knobState    = *knobState;
  _modeSwitch   = *modeSwitch;
  _motorState   = *motorState;
  _lightStage   = *lightStage;
  beat          = 0;
  lastBeat      = 7;
  poseLength    = DEFAULT_POSE_LENGTH;
  tempoModified = false;
  lastPose      = micros();
  lastClick     = micros();
}

void Posegram::init(){
    debug("Posegram is initializing");
    _motorState.init(); // motorState->init()
    _lightStage.init(); // lightStage->init()
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
 *  called from ISR on pin 2 (INT0)
 *  update how many millisecond occur between clicks for use in Posegram::makeTempo(),
 *  advance the beat index, rolling over back to 0 instead of advancing to 8.
 *  Also store the timing of the last click in order to debounce clicks (defined at the top of this file)
 *
 *  In the future, some hardware debouncing would be good.
 */
byte Posegram::nextBeat(){
  if(lastClick     < micros() - CLICK_DEBOUNCE){
      lastInterval = micros() - lastClick;
      lastClick    = micros();
      beat         = (beat + 1) % 8;
  }
}

void Posegram::programPosition(){
  _knobState.updateKnobs();

  if(lastBeat != beat){
    lastBeat = beat;
    _knobState.lockKnobs();
    temp = poses.get(beat);
    playThisPose();
  }

  if(_knobState.stage >= 0){
    temp.stage = _knobState.stage;
  }

  if(_knobState.backdrop >= 0){
    temp.backdrop = _knobState.backdrop;
  }

  if(_knobState.stage >= 0 or _knobState.backdrop >= 0){
    poses.put(beat, temp);
    // don't have to poses.get(beat), because we're updated the local temp struct anyway
    // poses.get(beat) would be identical to the temp struct that we just put.
    playThisPose();
  }
}

/* ## Posegram::playEachPose() 
 * If we find ourselves in this mode, we may have just left the programPosition mode,
 * so make sure we check for modifications and persist to EEPROM with `poses.saveChanges()`.
 * We're going to be using the knobs to adjust tempo and rate-of-change, but we should
 * ignore the knobs until they are moved, hence `_knobState.lockKnobs()`.
 *
 * In this mode we're checking the tempo (beats per minute), checking the clock,
 * and auto-advancing to the next beat if enough time has passed.
 *
 * Rememeber when knobState is -1, that means it hasn't changed since being locked.
 * So knobState >= 0 means that the knob is being adjusted and its value should be used.
 */

void Posegram::playEachPose(){
  poses.saveChanges();
  _knobState.lockKnobs();
  
  //_knobState.stage
  if(tempoModified){
    tempoModified = false;
    poseLength = (beatIntervals[0] + beatIntervals[1] + beatIntervals[2]) / 3;
  }

  if(lastPose < (micros() - poseLength)){
    debug("PoseLength is %lu", poseLength);
    lastPose = micros();
    nextBeat();
    playThisPose();
  }
}

void Posegram::playThisPose(){
    temp = poses.get(beat);
    _lightStage.updateBeat(beat, temp.stage, temp.backdrop);
    _motorState.updateMotors(temp.stage, temp.backdrop);
}

/* ## makeTempo()
 * 
 * In this mode, 
 * 
 */
void Posegram::makeTempo(){
  poses.saveChanges();
  switch(beat){
    case 0:
    case 1:
    case 2:
    case 3:
      // debug("advancing to five");
      beat = five;
      playThisPose();
      break;
    case six:
      // debug("beat is now six, saving interval one", lastInterval);
      beatIntervals[0] = lastInterval;
      playThisPose();
      break;
    case seven:
      // debug("beat is now seven, saving interval two", lastInterval);
      beatIntervals[1] = lastInterval;
      playThisPose();
      break;
    case eight:
      // debug("beat is now eight, saving interval three", lastInterval);
      beatIntervals[2] = lastInterval;
      playThisPose();
      break;
  }
  
  if(beatIntervals[0] > 0 and beatIntervals[1] > 0 and beatIntervals[2] > 0){
    tempoModified = true;
  }
  // debug("lastClick and lastInterval was: %12lu  ::   %12lu", lastClick, lastInterval);
  // debug("beat intervals are now:", beatIntervals, NUM_BEAT_INTERVALS);
}

#endif
