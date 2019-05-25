/*
 *
  #define MODESWITCH_PIN_A 4
  #define MODESWITCH_PIN_B 5
  
  ModeSwitch modeSwitch(MODESWITCH_PIN_A, MODESWITCH_PIN_B);
  void loop() {
    switch(modeSwitch.getModeState()){
      case 0: programPosition(); break;
      case 1: playEachPose();    break;
      case 2: followTheLeader(); break;
    }
  }
  
 */
#ifndef ModeSwitch_h
#define ModeSwitch_h

class ModeSwitch{
  public:
    ModeSwitch(int PIN_A, int PIN_B);
    ModeSwitch();
    byte getModeState();

  private:
    int UPSIDE;
    int DOWNSIDE;
};

ModeSwitch::ModeSwitch(){}
ModeSwitch::ModeSwitch(int PIN_A, int PIN_B){
  UPSIDE = PIN_A;
  DOWNSIDE = PIN_B;
  pinMode(UPSIDE, INPUT_PULLUP);
  pinMode(DOWNSIDE, INPUT_PULLUP);
}

byte ModeSwitch::getModeState(){
  if(digitalRead(UPSIDE) == LOW)
    return 0; // 'up', program mode
  else if(digitalRead(DOWNSIDE) == LOW)
    return 1; // 'down', play mode
  else 
    return 2; // 'middle', follow mode
}

#endif
