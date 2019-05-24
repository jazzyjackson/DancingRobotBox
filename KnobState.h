#ifndef KnobState_h
#define KnobState_h

#define LOCK_THRESHOLD 50

class KnobState {
  public:
    int backdrop;
    int stage;
    KnobState();
    KnobState(int x_pin, int y_pin);
    void updateKnobs();
    void lockKnobs();
  private:
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
KnobState::KnobState(int stage_pin, int backdrop_pin){
  _stage_pin = stage_pin;
  _backdrop_pin = backdrop_pin;
}

void KnobState::lockKnobs(){
  if(stage    >= 0){ _locked_stage    = stage; }
  if(backdrop >= 0){ _locked_backdrop = backdrop; }
  stage = -1;
  backdrop = -1;
}

void KnobState::updateKnobs(){
  _stage = analogRead(_stage_pin);
  _backdrop = analogRead(_backdrop_pin);

  if(backdrop >= 0 or backdropdiff() > LOCK_THRESHOLD){
    backdrop = _backdrop;
  }
  if(stage >= 0 or stagediff() > LOCK_THRESHOLD){
    stage = _stage;
  }
}

int KnobState::stagediff(){ return abs(_stage - _locked_stage); }
int KnobState::backdropdiff(){ return abs(_backdrop - _locked_backdrop); }

#endif
