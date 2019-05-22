/*
 * To get updated timings from interrupts
 * 
 * Example Usage:
  #define XPIN 2
  #define YPIN 3
  
  #include "DutyStruct.h"
  
  DutyCycle X = {XPIN, 0, 0, 0, 0};
  DutyCycle Y = {YPIN, 0, 0, 0, 0};
  
  void updateXHelper(){ X.dutyCycleUpdate(); }
  void updateYHelper(){ Y.dutyCycleUpdate(); }
  
  void setup() {
    attachInterrupt(digitalPinToInterrupt(X.pin), updateXHelper, CHANGE);
    attachInterrupt(digitalPinToInterrupt(Y.pin), updateYHelper, CHANGE);
  }
 * 
 * 
 */

#ifndef DutyCycle_h
#define DutyCycle_h

#include "Arduino.h"

typedef struct{
    byte pin;
    volatile short timeHigh;
    volatile short timeLow;
    volatile long lastFalling;
    volatile long lastRising;
    
    void dutyCycleUpdate(){
      switch(digitalRead(this->pin)){
        case HIGH:
          this->lastRising = micros();
          this->timeLow = this->lastRising - this->lastFalling;
          break;
        case LOW:
          this->lastFalling = micros();
          this->timeLow = this->lastFalling - this->lastRising;
          break;
      }
    }

    byte checkDutyCycle(){
      return map(this->timeHigh, 0, this->timeLow + this->timeHigh, 0, 255);
    }
    
} DutyStruct;

#endif
