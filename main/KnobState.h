#ifndef KnobState_h
#define KnobState_h

#include "debug.h"
#define LOCK_THRESHOLD 100

class KnobState {
  public:
    int backdrop;
    int stage;
    KnobState();
    KnobState(int x_pin, int y_pin, bool SWAP_STAGE_BACKDROP, bool STAGE_REVERSE_RANGE, bool BACKDROP_REVERSE_RANGE );
    void updateKnobs();
    void lockKnobs();
  private:
    bool _stage_range_reversed;
    bool _backdrop_range_reversed;
    int _stage;
    int _locked_stage;
    int _backdrop;
    int _locked_backdrop;
    int _stage_pin;
    int _backdrop_pin;
    int stagediff();
    int backdropdiff();
};

KnobState::KnobState(){}
KnobState::KnobState(int stage_pin, int backdrop_pin, bool SWAP_STAGE_BACKDROP, bool STAGE_REVERSE_RANGE, bool BACKDROP_REVERSE_RANGE ){
  if(SWAP_STAGE_BACKDROP){
    _stage_pin = backdrop_pin;
    _backdrop_pin = stage_pin;
  } else {
    _stage_pin = stage_pin;
    _backdrop_pin = backdrop_pin;
  }
  _stage_range_reversed = STAGE_REVERSE_RANGE;
  _backdrop_range_reversed = BACKDROP_REVERSE_RANGE;

}

void KnobState::lockKnobs(){
  if(stage    >= 0){ _locked_stage    = stage; }
  if(backdrop >= 0){ _locked_backdrop = backdrop; }
  stage = -1;
  backdrop = -1;
  // debug("KNOBS: Knobs locked, stage and backdrop should be negative one: %4d, %4d", stage, backdrop);
  // debug("KNOBS: Knobs locked, previous values for stage and backdrop is: %4d, %4d", _locked_stage, _locked_backdrop);
}

void KnobState::updateKnobs(){
  _stage    = analogRead(_stage_pin);
  _backdrop = analogRead(_backdrop_pin);

  if(_stage_range_reversed){ _stage = 1023 - _stage; }
  if(_backdrop_range_reversed){ _backdrop = 1023 - _backdrop;}

  if(backdrop >= 0 or backdropdiff() > LOCK_THRESHOLD){
    debug("KNOBS: new value for %8s: %4d", "backdrop", _backdrop);
    backdrop = _backdrop;
  }
  if(stage >= 0 or stagediff() > LOCK_THRESHOLD){
    debug("KNOBS: new value for %8s: %4d", "stage", _stage);
    stage = _stage;
  }
}

int KnobState::stagediff(){ return abs(_stage - _locked_stage); }
int KnobState::backdropdiff(){ return abs(_backdrop - _locked_backdrop); }

#endif
