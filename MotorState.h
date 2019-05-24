#ifndef MotorState_h
#define MotorState_h

#include <Adafruit_TiCoServo.h>
/* SG90 microservo requires pulses between 1 and 2 ms*/
#define SERVO_MIN 1000
#define SERVO_MAX 2000

class MotorState {
  private:
    Adafruit_TiCoServo servo_backdrop;
    Adafruit_TiCoServo servo_stage;

  public:
    MotorState(int servo_pin_x, int servo_pin_y);
    MotorState();
    void updateMotors(short stage, short backdrop);
};

MotorState::MotorState(){
  
}
MotorState::MotorState(int backdrop_pin, int stage_pin){

  servo_backdrop.attach(backdrop_pin);
  servo_stage.attach(stage_pin);
}


void MotorState::updateMotors(short stage, short backdrop){
  servo_stage.write(stage);
  servo_backdrop.write(backdrop);
}

#endif

// probably gonna have an x y struct so I can pass them around... maybe with .to8bit() .to10bit() to map precision...
