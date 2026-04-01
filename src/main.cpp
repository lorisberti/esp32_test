#include <Arduino.h>
#include "max7219.h"
#include <TFT_eSPI.h>

MAX7219 display;
TFT_eSPI tft = TFT_eSPI();

void setup() {
  display.Begin();
  display.Clear();
  display.MAX7219_SetBrightness(8);
  display.DisplayText("01234567", 0, true);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("ILI9225 OK", 10, 10, 2);

}

void loop() {
}