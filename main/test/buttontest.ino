
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
    Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(NEXTBEAT_PIN, INPUT_PULLUP);
  pinMode(MODESWITCH_PIN_B, INPUT_PULLUP);
  pinMode(MODESWITCH_PIN_A, INPUT_PULLUP);
  Serial.println("The push button and mode switch should all read 1 to begin with, and 0 when they are selected");
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Mode A: ");
  Serial.print(digitalRead(MODESWITCH_PIN_A));
  Serial.print("  Mode B: ");
  Serial.print(digitalRead(MODESWITCH_PIN_B));
  Serial.print("  Next Beat: ");
  Serial.println(digitalRead(NEXTBEAT_PIN));
  delay(10);
}
