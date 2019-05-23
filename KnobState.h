#ifndef KnobState_h
#define KnobState_h

#define LOCK_THRESHOLD 30

class KnobState {
  public:
    int x;
    int y;
    KnobState();
    KnobState(int x_pin, int y_pin);
    void updateKnobs();
    void lockKnobs();
  private:
    int _x;
    int _locked_x;
    int _y;
    int _locked_y;
    int _x_pin;
    int _y_pin;
    int xdiff();
    int ydiff();
};

KnobState::KnobState(){}
KnobState::KnobState(int x_pin, int y_pin){
  _x_pin = x_pin;
  _y_pin = y_pin;
}

void KnobState::lockKnobs(){
  if(x >= 0){ _locked_x = x; }
  if(y >= 0){ _locked_y = y; }
  x = -1;
  y = -1;
}

void KnobState::updateKnobs(){
  _x = analogRead(_x_pin);
  _y = analogRead(_y_pin);

  if(x >= 0 or xdiff() > LOCK_THRESHOLD){
    x = _x;
  }
  if(y >= 0 or ydiff() > LOCK_THRESHOLD){
    y = _y;
  }
}

int KnobState::xdiff(){ return abs(_x - _locked_x); }
int KnobState::ydiff(){ return abs(_y - _locked_y); }

#endif
