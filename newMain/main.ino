#include "ModeSwitch.h"  // handles 3-position throw switch
#include "KnobState.h"   // keeps the state of the knobs, X and Y
#include "LightStage.h"  // provides simple interface to NeoPixels, avoids unnecessary writes
#include "MotorState.h"  // writes positions to servos X and Y
#include "Posegram.h"    // interfaces with EEPROM to remember poses programmed, X and Y, beats 1-8.
#include "PinChangeInterrupt.h"

#define KNOB_STAGE         18 // ADC4 // innermost pot // PWR away from center, GND towards center
#define KNOB_BACKDROP      19 // ADC5 // outermost pot // PWR away from center, GND towards center
#define NEOPIXEL_PIN       6  // Clock 0 PWM - shared with MOTOR X, don't push too many simultaneous events
#define MODESWITCH_PIN_A   4 // digitalRead // aka UPSIDE, towards the arduino
#define MODESWITCH_PIN_B   8 // digitalRead // aka DOWNSIDE, away from the arduino
#define MOTOR_STAGE        9  // Clock 1 PWM
#define MOTOR_BACKDROP     10 // Clock 1 PWM
#define NEXTBEAT_PIN       2 // PCINT

// these booleans not yet used, would be great to set these to true to fix wiring accidents
bool NEOPIXEL_BACKWARDS          = false;
bool KNOB_SWAP_STAGE_BACKDROP    = false;
bool KNOB_STAGE_REVERSE_RANGE    = false;
bool KNOB_BACKDROP_REVERSE_RANGE = false;
bool MODESWITCH_UPSIDEDOWN       = false;

KnobState knobState = {
  KNOB_STAGE,  
  KNOB_BACKDROP
};

ModeSwitch modeSwitch(
  MODESWITCH_PIN_A,
  MODESWITCH_PIN_B
);

MotorState motorState(
  MOTOR_STAGE,
  MOTOR_BACKDROP
);

LightStage lightStage(
  NEOPIXEL_PIN
);

Posegram posegram(
  &knobState,
  &modeSwitch,
  &motorState,
  &lightStage
);

void nextBeatHelper(){ posegram.nextBeat(); }

void setup() {
  // put your setup code here, to run once:
  posegram.init();
  pinMode(NEXTBEAT_PIN, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(NEXTBEAT_PIN), nextBeatHelper, FALLING);
}

void loop() {
  switch(modeSwitch.getModeState()){
    case 0: posegram.programPosition(); break;
    case 1: posegram.playEachPose();    break;
    case 2: posegram.followTheLeader(); break;
  }
}
