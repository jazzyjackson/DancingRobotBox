#ifndef LightStage_h
#define LightStage_h

#include <Adafruit_NeoPixel.h>

// TODO add color to constructor

class LightStage {
  private:
    byte lastBeat;
    uint32_t beatColor;
    uint32_t lastBeatColor;
  public:
    Adafruit_NeoPixel strip;
    LightStage();
    LightStage(int pin);
    void updateBeat(byte beatNumber, int hue, int saturation, int brightness);
    void writeFill(uint32_t color);
};


LightStage::LightStage() {}
LightStage::LightStage(int pin) {
  lastBeat = 7;
  strip = Adafruit_NeoPixel(8, pin); // 8 beats, 8 LEDs
  strip.begin();
}

void LightStage::updateBeat(byte beatNumber, int hue, int saturation, int brightness) {
  beatColor = strip.ColorHSV(hue, saturation, brightness);
 // Serial.println(beatColor);
  if (beatNumber != lastBeat or beatColor != lastBeatColor) {
    lastBeat = beatNumber;
    strip.clear();
    strip.setPixelColor(beatNumber, beatColor);
    strip.show();
  }
}

void LightStage::writeFill(uint32_t color) {
  strip.fill(color);
}

#endif
