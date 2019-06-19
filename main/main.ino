#include "debug.h"
#include "ModeSwitch.h"  // handles 3-position throw switch
#include "KnobState.h"   // keeps the state of the knobs, X and Y
#include "LightStage.h"  // provides simple interface to NeoPixels, avoids unnecessary writes
#include "MotorState.h"  // writes positions to servos X and Y
#include "Posegram.h"    // interfaces with EEPROM to remember poses programmed, X and Y, beats 1-8.

#define KNOB_STAGE         18 // ADC4 // innermost pot // PWR away from center, GND towards center
#define KNOB_BACKDROP      19 // ADC5 // outermost pot // PWR away from center, GND towards center
#define NEOPIXEL_PIN       6  // Clock 0 PWM - shared with MOTOR X, don't push too many simultaneous events
#define MODESWITCH_PIN_A   4 // digitalRead // aka UPSIDE, towards the arduino
#define MODESWITCH_PIN_B   8 // digitalRead // aka DOWNSIDE, away from the arduino
#define MOTOR_STAGE        9  // Clock 1 PWM
#define MOTOR_BACKDROP     10 // Clock 1 PWM
#define NEXTBEAT_PIN       2 // PCINT

bool NEOPIXEL_BACKWARDS          = true;
bool KNOB_SWAP_STAGE_BACKDROP    = false;
bool KNOB_STAGE_REVERSE_RANGE    = false;
bool KNOB_BACKDROP_REVERSE_RANGE = false;
bool MODESWITCH_UPSIDEDOWN       = false;
bool MOTORS_SWAP_STAGE_BACKDROP  = false;

KnobState knobState = {
  KNOB_STAGE,  
  KNOB_BACKDROP,
  KNOB_SWAP_STAGE_BACKDROP,
  KNOB_STAGE_REVERSE_RANGE,
  KNOB_BACKDROP_REVERSE_RANGE
};

ModeSwitch modeSwitch(
  MODESWITCH_PIN_A,
  MODESWITCH_PIN_B,
  MODESWITCH_UPSIDEDOWN
);

MotorState motorState(
  MOTOR_STAGE,
  MOTOR_BACKDROP,
  MOTORS_SWAP_STAGE_BACKDROP
);

LightStage lightStage(
  NEOPIXEL_PIN,
  NEOPIXEL_BACKWARDS
);

Posegram posegram(
  &knobState,
  &modeSwitch,
  &motorState,
  &lightStage
);

void nextBeatHelper(){ posegram.nextBeat(); }

void setup() {
  // Serial.begin(9600); // uncomment Serial.begin to print debug statements. Makes a lot of the timing a little worse to be sending so many characters over Serial.
  debug("SETUP: Entering Setup");
  posegram.init();
  pinMode(NEXTBEAT_PIN, INPUT_PULLUP);
  attachInterrupt(0, nextBeatHelper, FALLING);
  debug("SETUP: Exiting Setup");
}

void loop() {
  switch(modeSwitch.getModeState()){
    case 0: posegram.programPosition(); break;
    case 1: posegram.makeTempo();       break;
    case 2: posegram.playEachPose();    break;
  }
}
