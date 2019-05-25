#ifndef MotorState_h
#define MotorState_h

#include <Adafruit_TiCoServo.h>

class MotorState {
  private:
    Adafruit_TiCoServo servo_backdrop;
    Adafruit_TiCoServo servo_stage;

  public:
    MotorState(int servo_pin_x, int servo_pin_y);
    MotorState();
    void updateMotors(int stage, int backdrop);
};

MotorState::MotorState(){}

MotorState::MotorState(int backdrop_pin, int stage_pin){
  servo_backdrop.attach(backdrop_pin);
  servo_stage.attach(stage_pin); 
}


void MotorState::updateMotors(int backdrop, int stage){
  int s = constrain(map(stage, 0, 1023, 500, 2500), 500, 2500);
  int b = constrain(map(backdrop, 0, 1023, 500, 2500), 500, 2500);
//  Serial.print(s);
//  Serial.print(" ");
//  Serial.println(b);
  servo_stage.write(s);
  delay(20);
  servo_backdrop.write(b);
}

#endif

// probably gonna have an x y struct so I can pass them around... maybe with .to8bit() .to10bit() to map precision...
