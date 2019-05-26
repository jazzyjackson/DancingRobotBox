#ifndef MotorState_h
#define MotorState_h

#include <Adafruit_TiCoServo.h>

class MotorState {
  private:
    int _backdrop_pin;
    int _stage_pin;
    Adafruit_TiCoServo servo_backdrop;
    Adafruit_TiCoServo servo_stage;

  public:
    MotorState();
    void init(int stage_pin, int backdrop_pin);
    void updateMotors(int stage, int backdrop);
};

MotorState::MotorState(){}


void MotorState::init(int stage_pin, int backdrop_pin){

  servo_stage.attach(stage_pin);
  servo_backdrop.attach(backdrop_pin);
    
}


void MotorState::updateMotors(int stage, int backdrop){
  stage = map(stage, 0, 1023, 0, 180);
  backdrop = map(backdrop, 0, 1023, 0, 180);
  servo_stage.write(stage);
  servo_backdrop.write(backdrop);
}

#endif

// probably gonna have an x y struct so I can pass them around... maybe with .to8bit() .to10bit() to map precision...
