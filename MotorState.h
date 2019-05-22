#ifndef MotorState_h
#define MotorState_h

class MotorState {
  private:
    int x_pin;
    int y_pin;
    int x;
    int y;

  public:
    MotorState(int servo_pin_x, int servo_pin_y);
    MotorState();
    void updateMotors(int x_pos, int y_pos);
};

MotorState::MotorState(){
  
}
MotorState::MotorState(int servo_pin_x, int servo_pin_y){
  x_pin = servo_pin_x;
  y_pin = servo_pin_y;
}

void MotorState::updateMotors(int x_pos, int y_pos){
//      servo_x.write(x_pos);
//      servo_y.write(y_pos);
}

#endif

// probably gonna have an x y struct so I can pass them around... maybe with .to8bit() .to10bit() to map precision...
