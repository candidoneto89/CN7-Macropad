/*
 CN7 Macropad Layout:

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
 
#include "HID-Project.h"
#include <Adafruit_NeoPixel.h>
#define PIN 14 //MISO PIN
#define NUM 18
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM,PIN, NEO_GRB + NEO_KHZ800);

int macropadPins[6] = {A2, A1, A0, A5, A4, A3};
int macropadRows[3] = {A2, A1, A0};
int macropadColumns[3] = {A5, A4, A3};

int encoderA = 9;
int encoderB = 13;
int encoderC = 6;

int led1 = 5; // PD2 (pin 0 on Leonardo)
int led2 = 0; // PD3 (pin 1 on Leonardo)
int led3 = 1;

int layerMode = 1;

int macropadRead;  // Used to monitor which buttons are pressed.
int timeout;  // timeout variable used in loop
char keys[3][3] = {
  {'7','8','9'},
  {'5','6','4'},
  {'1','2','3'},
};

void setup()
{
  Serial.begin(9600);
  Consumer.begin();
  Keyboard.begin();
  pixels.begin();
 
  for (int i=0; i<6; i++)
  {
    pinMode(macropadPins[i], INPUT);  // Set all keypad pins as inputs
    digitalWrite(macropadPins[i], HIGH);  // pull all keypad pins high
  }
  pinMode(encoderA, INPUT);
  pinMode(encoderB, INPUT);
  pinMode(encoderC, INPUT_PULLUP);

  digitalWrite(encoderA, HIGH); //turn pullup on
  digitalWrite(encoderB, HIGH); //turn pullup on
  digitalWrite(encoderC, HIGH); //turn pullup on

  DDRC = DDRC | B01000000;// setando C6 como saída sem influenciar nos demais pinos
  PORTC = B00000000;
  DDRD = DDRD | B00001100;  // setando D2 e D3 como saída sem influenciar nos demais pinos
  PORTD = B00000000;
  
  // Configurando Timer Interrupt:
  cli();
  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1B |= B00000100;
  TIMSK1 |= B00000010;
  OCR1A = 1000;
  sei();
  //
  
  static1(120,0,0);
  delay(1000);
  static1(0,120,0);
  delay(1000);
  static1(0,0,120);
  delay(1000);
  static1(120,25,0);
 
}

void loop()
{
  rgbLoop();
 
  
}

ISR(TIMER1_COMPA_vect){
  TCNT1  = 0; 

  if(readEncButton()) {
    if (layerMode == 3) layerMode=1;
    else layerMode++;
  }

  switch(layerMode) {
    case 1:
      digitalWrite(led1, LOW);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      encoder1();
      break;    
    case 2:
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      digitalWrite(led3, HIGH);
      encoder2();
      break;
    case 3:
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
      encoder3();
      break;
  }
 
  /*//////////////////////////////////////////////////////////////////////////////////////*/
  macropadRead = readKeypad();  // read which buttons are pressed
  if (macropadRead)  // If a button is pressed go into here
  {
    Serial.print(macropadRead);
    sendKeyPress(macropadRead);  // send the button over USB
    timeout = 2000;  // top of the repeat delay
    while ((readKeypad() == macropadRead) && (--timeout))  // Decrement timeout and check if key is being held down
      delayMicroseconds(1);
    while (readKeypad() == macropadRead)  // while the same button is held down
    {
      //Serial.print(macropadRead);
      sendKeyPress(macropadRead);  // continue to send the button over USB
      delay(50);  // 50ms repeat rate
    }
  }
}

/*//////////////////////////////////////////////////////////////////////////////////////*/

boolean readEncButton(void){
  if(!digitalRead(encoderC)){
    delay(50);
    if(!digitalRead(encoderC)) {
      while(!digitalRead(encoderC));
      return true;
    }
  }
  return false;
}

/*//////////////////////////////////////////////////////////////////////////////////////*/

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
  setAll(128,0,0);
  pixels.show();
  delay(t);
  // red + green
  for(int k = 0; k < 128; k++) {
    setAll(128,k,0);
    pixels.show();
    delay(t);
  }
  // green
  for(int k = 128; k > 0; k--) {
    setAll(k,128,0);
    pixels.show();
    delay(t);
  }
  // green + blue
  for(int k = 0; k < 128; k++) {
    setAll(0,128,k);
    pixels.show();
    delay(t);
  }
  // blue
  for(int k = 128; k > 0; k--) {
    setAll(0,k,128);
    pixels.show();
    delay(t);
  }
  // blue + red
  for(int k = 0; k < 128; k++) {
    setAll(k,0,128);
    pixels.show();
    delay(t);
  }  
  // red
  for(int k = 128; k > 0; k--) {
    setAll(128,0,k);
    pixels.show();
    delay(t);
  }
}

/*//////////////////////////////////////////////////////////////////////////////////////*/

void sendKeyPress(char key)
{
  switch(layerMode) {
    case 1:
      sendKeyPress1(key);
      break;
    case 2:
      sendKeyPress2(key);
      break;
    case 3:
      sendKeyPress3(key);
      break;    
  }
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
      Keyboard.write('3');
      break;
    case '4':  // 0x008
      Keyboard.write('4');
      break;
    case '5':  // 0x010
      //Keyboard.write('5');
      break;
    case '6':  // 0x020
      //Keyboard.write('6');
      break;
    case '7':  // 0x040
      //Keyboard.write('7');
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('t');
      Keyboard.releaseAll();
      break;
    case '8':  // 0x080
      //Keyboard.write('8');
            Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('w');
      Keyboard.releaseAll();
      break;
    case '9':  // 0x100
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.write('t');
      Keyboard.releaseAll();
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
      Keyboard.write('8');
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
      Keyboard.write('8');
      break;
  }
}

/*//////////////////////////////////////////////////////////////////////////////////////*/

void encoder1() {
  if (digitalRead(encoderA) == LOW)
  {
    if (digitalRead(encoderB) != LOW)
    {
//      Serial.println("D");
      Consumer.write(MEDIA_VOL_DOWN);
    }
    else
    {
//      Serial.println("U");
      Consumer.write(MEDIA_VOL_UP);
    }
    // Wait until rotation to next detent is complete
    while (digitalRead(encoderA) != HIGH) delay(5); // Debounce delay
  }
}

void encoder2() {
  if (digitalRead(encoderA) == LOW)
  {
    if (digitalRead(encoderB) != LOW)
    {
//      Serial.println("D");
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('-');
      Keyboard.releaseAll();  
    }
    else
    {
//      Serial.println("U");
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.write('+');
      Keyboard.releaseAll();  
    }
    // Wait until rotation to next detent is complete
    while (digitalRead(encoderA) != HIGH) delay(5); // Debounce delay
  }
}

void encoder3() { encoder1(); }

/*//////////////////////////////////////////////////////////////////////////////////////*/

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
    pinMode(macropadRows[row], OUTPUT);  // set the row pin as an output
    digitalWrite(macropadRows[row], LOW);  
    for (int col=0; col<3; col++)
    {  
      if (!digitalRead(macropadColumns[col])) result = keys[row][col];
    }
    digitalWrite(macropadRows[row], HIGH);
  }
  return result;
}
