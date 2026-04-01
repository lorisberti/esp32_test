#include <Arduino.h>
#include "max7219.h"

MAX7219 display;

void setup() {
  display.Begin();
  display.Clear();
  display.MAX7219_SetBrightness(8);

  // Test: accende una cifra alla volta
  for (int digit = 0; digit < 8; digit++) {
    display.Clear();
    display.DisplayChar(digit, '8', false);  // accende tutti i segmenti della cifra
    delay(500);
  }

  display.Clear();
}

void loop() {
}