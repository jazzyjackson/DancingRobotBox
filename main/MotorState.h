#ifndef MotorState_h
#define MotorState_h

#include "KnobState.h"

typedef struct {
  int h;
  int s;
  int b;
} colorHSV;

class MotorState {
  private:
    int x_pin;
    int y_pin;
    int x;
    int y;

  public:
    MotorState(int servo_pin_x, int servo_pin_y);
    MotorState();
    void updateMotors(KnobState *knobState);
};

MotorState::MotorState(){
  
}
MotorState::MotorState(int servo_pin_x, int servo_pin_y){
  x_pin = servo_pin_x;
  y_pin = servo_pin_y;
}

void MotorState::updateMotors(KnobState *knobState){
//  if(*knobState->x >= 0){ servo_x.write(*knobState->x) };
//  if(*knobState->y >= 0){ servo_y.write(*knobState->y) };
}

#endif

// probably gonna have an x y struct so I can pass them around... maybe with .to8bit() .to10bit() to map precision...
