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

#define clickDebounce 200
#define defaultTempo 120
#define NUM_BEAT_INTERVALS 3

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
    volatile byte  beat;         // 8 bit, max 255, only holds 0 - 7
    volatile int   lastInterval; // 16bit, max 32,767, maxes out at 32 seconds between beats. unsigned would yeiled 64 seconds.
    volatile long  lastClick;    // 32bit, max 2,147,483,647, about 24 days worth of milliseconds before we have to think about rollover.

    int poseLength;
    int tempo;
    int tempoSum;
    bool tempoModified;
    byte lastBeat;
    long lastPose;
    int beatIntervals[NUM_BEAT_INTERVALS];

    void playThisPose(byte poseIndex);
    void initPoses(int initValue);
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
  tempo         = defaultTempo;
  tempoModified = false;
  lastPose      = millis();
  lastClick     = millis();
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
  if(lastClick     < millis() - clickDebounce){
      lastInterval = millis() - lastClick;
      lastClick    = millis();
      beat         = (beat + 1) % 8;
  }
}

void Posegram::programPosition(){
  _knobState.updateKnobs();

  if(lastBeat != beat){
    lastBeat = beat;
    _knobState.lockKnobs();
    temp = poses.get(beat);
    playThisPose(beat);
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
    playThisPose(beat);
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
  poseLength = 60000 / tempo; 
  
  //_knobState.stage
  if(tempoModified){
    tempoModified = false;
    tempoSum = beatIntervals[0] + beatIntervals[1] + beatIntervals[2];
    // tempoSum / 3 will be the average interval of 3 readings.
    // 60000 / tempoSum / 3 will be the beats per minute used....
    tempo = 60000 / (tempoSum / 3);

    if(tempo < 30 or tempo > 200){
      tempo = defaultTempo;
    }
  }

  if(lastPose < (millis() - poseLength)){
    lastPose = millis();
    nextBeat();
    playThisPose(beat);
  }
}

void Posegram::playThisPose(byte poseIndex){
    temp = poses.get(beat);
    _lightStage.updateBeat(beat, temp.stage, temp.backdrop);
    _motorState.updateMotors(temp.stage, temp.backdrop);
}

/* ## makeTempo()
 * 
 * In this mode, 
 */
void Posegram::makeTempo(){
  poses.saveChanges();
  _knobState.lockKnobs();
  switch(beat){
    case 0:
    case 1:
    case 2:
    case 3:
      beat = five;
      break;
    case six:
      beatIntervals[0] = lastInterval;
      playThisPose(six);
      break;
    case seven:
      beatIntervals[1] = lastInterval;
      playThisPose(seven);
      break;
    case eight:
      beatIntervals[2] = lastInterval;
      playThisPose(eight);
      break;
  }
  tempoModified = true;
  debug("beat intervals are now:", beatIntervals, NUM_BEAT_INTERVALS);
}

#endif
