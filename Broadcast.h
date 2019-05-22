#ifndef Broadcast_h
#define Broadcast_h

#include "Arduino.h"

class Broadcast{
  public:
    Broadcast(int stage_pin, int backdrop_pin);
    Broadcast();
    void updateOutput(int stage_out, int backdrop_out);
  private:
    int _stage_pin;
    int _backdrop_pin;
};

Broadcast::Broadcast(){}
Broadcast::Broadcast(int stage_pin, int backdrop_pin){
  _stage_pin = stage_pin;
  _backdrop_pin = backdrop_pin;
}

void Broadcast::updateOutput(int stage_out, int backdrop_out){
  analogWrite(_stage_pin, stage_out);
  analogWrite(_backdrop_pin, backdrop_out);
}

#endif
