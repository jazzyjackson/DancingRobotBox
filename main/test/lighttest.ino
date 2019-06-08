
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

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(8, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    Serial.begin(9600);
  // initialize serial communication at 9600 bits per second:
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.setBrightness(50);
}

void loop() {
  rainbow(5);
}


void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}
