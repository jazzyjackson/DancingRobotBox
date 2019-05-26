#ifndef LightStage_h
#define LightStage_h


#include <Adafruit_NeoPixel.h>

#define saturation 255
#define stage_brightness 120
#define backdrop_brightness 50

/*
 * Controls a strip of 8 neopixels, initializing via constructor with just the pin number.
 * Expects to be updated with a beat number and 10bit (0-1023) values to update the hue
 * one hue for the pixel representing the current 'beat', another to fill the backdrop.
 */
class LightStage {
  public:
    Adafruit_NeoPixel strip;
    LightStage();
    LightStage(int pin);
    void updateBeat(byte beatNumber, short backdropHue, short stageHue);
  private:
    byte lastBeat;
    short lastBackdropHue;
    short lastStageHue;
    uint32_t backdropColor;
    uint32_t stageColor;
    short convert10to16bit(short value);
};

LightStage::LightStage() {}
LightStage::LightStage(int pin) {
  lastBeat = 7;
  strip = Adafruit_NeoPixel(8, pin); // 8 beats, 8 LEDs
  strip.begin();
}

short LightStage::convert10to16bit(short value){
  // 2^10 = 1024, 2^16 = 65536
  return map(value, 0, 1023, 1, 65536);
}

/**
 * byte is expected to be a beat 0 - 7
 * backdrop and stage hue are expected to be passed from a pose, a 10bit value recorded from a knob
 * 
 * updateBeat checks if the beat number, backdropHue, or stageHue have changed since
 * last time the function was called, and if so, maps the hues to gamma corrected colors
 * at the pre-defined saturation and brightnes (top of this file)
 * fills the neopixel strip with backdropColor, sets the pixel correponding with the beat to stage color
 */
void LightStage::updateBeat(byte beatNumber, short backdropHue, short stageHue) {
  
  if (beatNumber != lastBeat or backdropHue != lastBackdropHue or stageHue != lastStageHue) {
    
    backdropHue = convert10to16bit(backdropHue);
    stageHue = convert10to16bit(stageHue);
    backdropColor = strip.gamma32(strip.ColorHSV(backdropHue, saturation, backdrop_brightness));
    stageColor    = strip.gamma32(strip.ColorHSV(stageHue, saturation, stage_brightness));

    lastBeat = beatNumber;

    strip.clear();
    strip.fill(backdropColor);
    strip.setPixelColor(beatNumber, stageColor);
    strip.show();
    
  }
  
}

#endif
