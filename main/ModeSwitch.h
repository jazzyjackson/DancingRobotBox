#ifndef ModeSwitch_h
#define ModeSwitch_h

class ModeSwitch{
  public:
    ModeSwitch(int PIN_A, int PIN_B, bool UPSIDEDOWN);
    ModeSwitch();
    byte getModeState();

  private:
    int UPSIDE;
    int DOWNSIDE;
};

ModeSwitch::ModeSwitch(){}
ModeSwitch::ModeSwitch(int PIN_A, int PIN_B, bool UPSIDEDOWN){
  if(UPSIDEDOWN){
    UPSIDE = PIN_A;
    DOWNSIDE = PIN_B;
  } else {
    DOWNSIDE = PIN_A;
    UPSIDE = PIN_B;
  }
  pinMode(UPSIDE, INPUT_PULLUP);
  pinMode(DOWNSIDE, INPUT_PULLUP);
}

/*  __   Mode Switch can be one of three positions
 * |  |/ 0 "UP" position, used to program the poses
   |  |- 1 "MIDDLE" position, for setting the tempo
   |__|\ 2 "DOWN" position, to play all the poses
 */

byte ModeSwitch::getModeState(){
  if(digitalRead(UPSIDE) == LOW)
    return 0; // 'up', program mode
  else if(digitalRead(DOWNSIDE) == LOW)
    return 2; // 'down', play mode
  else 
    return 1; // 'middle', tempo mode
}

#endif
