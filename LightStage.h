#include <Adafruit_NeoPixel.h>

// TODO add color to constructor

class LightStage {
  private:
    byte lastBeat;
    Adafruit_NeoPixel strip;
  public:
    LightStage(int pin);
    void writeBeat(byte beatNumber);
//    void writeFill(color);
};

LightStage::LightStage(int pin){
  lastBeat = 7;
  strip = Adafruit_NeoPixel(8, pin); // 8 beats, 8 LEDs
  strip.begin();
}

void LightStage::writeBeat(byte beatNumber){
  if(beatNumber != lastBeat){
    lastBeat = beatNumber;
    strip.clear();
    strip.setPixelColor(beatNumber, 100, 10, 10);
    strip.show();
  }
}

//void LightStage::writeFill(color){
//  strip.fill(color)
//}

//void nextBeat(){
//  if(recentClick < millis() - 200){
//      beat = (beat + 1) % 8;
//  }
//  recentClick = millis();
//}
