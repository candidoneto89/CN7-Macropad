#include <Adafruit_NeoPixel.h>
#include "led_animations.h"
#define PIN 14 //MISO PIN
#define NUM 18
#define LUM 64

extern Adafruit_NeoPixel pixels;

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM; i++ ) {
    pixels.setPixelColor(i, pixels.Color(red, green, blue));
  }
  pixels.show();
}

void static1(int r, int g, int b)
{
  for(int i=0;i<=NUM;i++)
  {
  pixels.setPixelColor(i, pixels.Color(r,g,b));
  pixels.show();
  }
}

void rgbLoop() {
  int t=4;
  // start red
  setAll(LUM,0,0);
  pixels.show();
  delay(t);
  // red + green
  for(int k = 0; k < LUM; k++) {
    setAll(LUM,k,0);
    pixels.show();
    delay(t);
  }
  // green
  for(int k = LUM; k > 0; k--) {
    setAll(k,LUM,0);
    pixels.show();
    delay(t);
  }
  // green + blue
  for(int k = 0; k < LUM; k++) {
    setAll(0,LUM,k);
    pixels.show();
    delay(t);
  }
  // blue
  for(int k = LUM; k > 0; k--) {
    setAll(0,k,LUM);
    pixels.show();
    delay(t);
  }
  // blue + red
  for(int k = 0; k < LUM; k++) {
    setAll(k,0,LUM);
    pixels.show();
    delay(t);
  }  
  // red
  for(int k = LUM; k > 0; k--) {
    setAll(LUM,0,k);
    pixels.show();
    delay(t);
  }
}