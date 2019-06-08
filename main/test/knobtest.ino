
#define KNOB_STAGE         A4 // ADC4 // innermost pot // PWR away from center, GND towards center
#define KNOB_BACKDROP      A5 // ADC5 // outermost pot // PWR away from center, GND towards center
#define PWM_INPUT_X        2  // INT0
#define PWM_INPUT_Y        3  // INT1
#define MODESWITCH_PIN_A   8 // digitalRead // aka DOWNSIDE, away from the arduino
#define BROADCAST_STAGE    5  // Clock 0 PWM
#define NEOPIXEL_PIN       6  // Clock 0 PWM - shared with MOTOR X, don't push too many simultaneous events
#define MODESWITCH_PIN_B   4 // digitalRead  // aka UPSIDE, towards the arduino
#define MOTOR_STAGE        9  // Clock 1 PWM
#define MOTOR_BACKDROP     10 // Clock 1 PWM
#define BROADCAST_BACKDROP 11 // Clock 2 PWM
#define NEXTBEAT_PIN       2 // PCINT

void setup() {
  // put your setup code here, to run once:
  // put your main code here, to run repeatedly:
  Serial.begin(9600);
  Serial.println("KNOB_STAGE should be towards the center, and sweep 0 to 1023 from right to left");
  Serial.println("KNOB_BACKDROP should be away from the center, and sweep 0 to 1023 from right to left");
  delay(1000);
}

void loop() {
  // print out the value you read:
  Serial.print("STAGE: ");
  Serial.print(analogRead(KNOB_STAGE));
  Serial.print("   BACKDROP: ");
  Serial.println(analogRead(KNOB_BACKDROP));
  delay(10);        // delay in between reads for stability
}
