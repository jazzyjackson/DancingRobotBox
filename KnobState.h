#ifndef KnobState_h
#define KnobState_h

typedef struct {
  int backdrop_pin;
  int stage_pin;
  int stage;
  int backdrop;

  void updateKnobs(){
    this->backdrop = analogRead(this->backdrop_pin);
    this->stage = analogRead(this->stage_pin);
  }
} KnobState;

// knobState.stage will return the last updated x value,
// knobState.backdrop will return the last updated y value... 

#endif
