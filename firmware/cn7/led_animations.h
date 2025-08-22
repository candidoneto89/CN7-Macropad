#ifndef LED_ANIMATIONS_H
#define LED_ANIMATIONS_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

void setAll(byte red, byte green, byte blue);
void static1(int r, int g, int b);
void rgbLoop();

#endif