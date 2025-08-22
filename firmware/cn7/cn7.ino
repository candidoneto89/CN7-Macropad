/*
Keys Layout:

 K7 K8 K9
 K5 K6
 K3 K4
 -  -
 K1 K2

 Matrix Layout:

 K7 K8 K9 - A2
 K5 K6 K4 - A1
 K1 K2 K3 - A0
 |  |  |
 A5 A4 A3 (Leonardo pins)
 
*/

#include <Adafruit_NeoPixel.h> 
#include <RotaryEncoder.h>
#include "HID-Project.h"
#include "led_animations.h"

#define PIN 14 //MISO PIN
#define NUM 18

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM, PIN, NEO_GRB + NEO_KHZ800);

int macropadPins[6] = {A2, A1, A0, A5, A4, A3};
int macropadRows[3] = {A2, A1, A0};
int macropadColumns[3] = {A5, A4, A3};

// Encoder pins:
int encoderA = 9;
int encoderB = 13;
int encoderC = 6;
volatile long lastPos = 0;

RotaryEncoder encoder(encoderA, encoderB);

// Layer indicator LED pins:
int led1 = 5; // PC6 (pin 5 on Leonardo)
int led2 = 0; // PD2 (pin 0 on Leonardo)
int led3 = 1; // PD3 (pin 1 on Leonardo)
const int leds[] = {led1, led2, led3};

int layerMode = 1;

int macropadRead;  // Used to monitor which buttons are pressed.
int timeout;  // timeout variable used in loop
char keys[3][3] = {
  {'7','8','9'},
  {'5','6','4'},
  {'1','2','3'},
};

void sendKeyPress1(char key);
void sendKeyPress2(char key);
void sendKeyPress3(char key);
void (*sendKeyFuncs[])(char) = { nullptr, sendKeyPress1, sendKeyPress2, sendKeyPress3 };

void setup()
{
  Serial.begin(9600);
  Consumer.begin();
  Keyboard.begin();
  pixels.begin();
 
  for (int i=0; i<6; i++)
  {
    pinMode(macropadPins[i], INPUT);      // Set all keypad pins as inputs
    digitalWrite(macropadPins[i], HIGH);  // Pull all keypad pins high
  }

  // Setting the layer indicator LEDs pins as output
  DDRC = DDRC | B01000000;  
  PORTC = B00000000;
  DDRD = DDRD | B00001100;  
  PORTD = B00000000;
  
  // Setting up Timer Interrupt:
  cli();
  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1B |= B00000100;
  TIMSK1 |= B00000010;
  OCR1A = 1000;
  sei();
}

void loop()
{
  rgbLoop();
}

ISR(TIMER1_COMPA_vect){
  TCNT1  = 0; 

  encoder.tick();

  if(readEncButton()) {
    if (layerMode == 3) layerMode=1;
    else layerMode++;
  }

  switch(layerMode) {
    case 1:
      setLed(1);
      encoder1();
      break;    
    case 2:
      setLed(2);
      encoder2();
      break;
    case 3:
      setLed(3);
      encoder3();
      break;
  }
 
  macropadRead = readKeypad();  // read which buttons are pressed
  if (macropadRead)             // if a button is pressed 
  {
    sendKeyFuncs[layerMode](macropadRead);
    // sendKeyPress(macropadRead);  // send the button over USB
    timeout = 2000;              // top of the repeat delay
    while ((readKeypad() == macropadRead) && (--timeout))  // Decrement timeout and check if key is being held down
      delayMicroseconds(1);
    while (readKeypad() == macropadRead)  // while the same button is held down
    {
      sendKeyFuncs[layerMode](macropadRead);
      // sendKeyPress(macropadRead);  // continue to send the button over USB
      delay(50);  // 50ms repeat rate
    }
  }
}

void setLed(int index) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(leds[i], i == (index-1) ? LOW : HIGH);
  }
}


boolean readEncButton(void)
{
  if(!digitalRead(encoderC)){
    delay(50);
    if(!digitalRead(encoderC)) {
      while(!digitalRead(encoderC));
      return true;
    }
  }
  return false;
}

void sendKeyPress1(char key)
{
  switch(key)
  {
    case '1':  // 0x001
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('c');
      Keyboard.releaseAll();  
      break;
    case '2':  // 0x002
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('v');
      Keyboard.releaseAll();  
      break;
    case '3':  // 0x004
      //Keyboard.write('3');
      break;
    case '4':  // 0x008
      //Keyboard.write('4');
      break;
    case '5':  // 0x010
      //Keyboard.write('5');
      break;
    case '6':  // 0x020
      //Keyboard.write('6');
      break;
    case '7':  // 0x040
      //Keyboard.write('7');
      break;
    case '8':  // 0x080
      Keyboard.write('8');
      break;
    case '9':  // 0x100
      Keyboard.write('9');
      break;
  }
}

void sendKeyPress2(char key)
{
  switch(key)
  {
    case '1':  // 0x001
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('+');
      Keyboard.releaseAll();  
      break;
    case '2':  // 0x002
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('-');
      Keyboard.releaseAll();  
      break;
    case '3':  // 0x004
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('0');
      Keyboard.releaseAll();  
      break;
    case '4':  // 0x008
      //Keyboard.write('4');
      break;
    case '5':  // 0x010
      //Keyboard.write('5');
      break;
    case '6':  // 0x020
      //Keyboard.write('6');
      break;
    case '7':  // 0x040
      //Keyboard.write('7');
      break;
    case '8':  // 0x080
      //Keyboard.write('8');
      break;
    case '9':  // 0x100
      //Keyboard.write('9');
      break;
  }
}

void sendKeyPress3(char key)
{
  switch(key)
  {
    case '1':  // 0x001
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('c');
      Keyboard.releaseAll();  
      break;
    case '2':  // 0x002
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('v');
      Keyboard.releaseAll();  
      break;
    case '3':  // 0x004
      //Keyboard.write('3');
      break;
    case '4':  // 0x008
      //Keyboard.write('4');
      break;
    case '5':  // 0x010
      //Keyboard.write('5');
      break;
    case '6':  // 0x020
      //Keyboard.write('6');
      break;
    case '7':  // 0x040
      //Keyboard.write('7');
      break;
    case '8':  // 0x080
      //Keyboard.write('8');
      break;
    case '9':  // 0x100
      //Keyboard.write('9');
      break;
  }
}

void encoder1() {
  long newPos = encoder.getPosition();
  if (newPos != lastPos) {
    if (newPos > lastPos) {
      Consumer.write(MEDIA_VOL_UP);
    } else {
      Consumer.write(MEDIA_VOL_DOWN);
    }
    lastPos = newPos;
  }
}

void encoder2() {
  long newPos = encoder.getPosition();
  if (newPos != lastPos) {
    if (newPos > lastPos) {
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('-');
      Keyboard.releaseAll(); 
    } else {
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('+');
      Keyboard.releaseAll(); 
    }
    lastPos = newPos;
  }
}

void encoder3() {
  long newPos = encoder.getPosition();
  if (newPos != lastPos) {
    if (newPos > lastPos) {
      Consumer.write(MEDIA_VOL_UP);
    } else {
      Consumer.write(MEDIA_VOL_DOWN);
    }
    lastPos = newPos;
  }
}

char readKeypad()
{
  char result = 0;  
 
  for (int i=0; i<6; i++)
  {
    pinMode(macropadPins[i], INPUT);
    digitalWrite(macropadPins[i], HIGH);
  }
 
  for (int row=0; row<3; row++)
  {
    pinMode(macropadRows[row], OUTPUT);  
    digitalWrite(macropadRows[row], LOW);  
    for (int col=0; col<3; col++)
    {  
      if (!digitalRead(macropadColumns[col])) result = keys[row][col];
    }
    digitalWrite(macropadRows[row], HIGH);
  }
  return result;
}
