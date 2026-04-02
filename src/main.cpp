/*
for pin configuration check:
MAX7219_7Segment/max7219.h
TFT_eSPI/User_setup.h
*/

/*
TODO: the commented part is not making work the MAX7219 part
*/

#include <Arduino.h>
#include "max7219.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <ESP32TimerInterrupt.h>

MAX7219 display;
TFT_eSPI tft = TFT_eSPI();

ESP32Timer timer0(0);
volatile bool timerTicked = false;
volatile unsigned long timerCount = 0;
char timerText[9] = "00000000";

bool IRAM_ATTR onTimer(void *param)
{
  (void) param;
  timerTicked = true;
  timerCount++;
  return true;
}

void setup() {

  display.Begin();
  display.Clear();
  display.MAX7219_SetBrightness(8);
  display.DisplayText(timerText, 0, true);

  // tft.begin();
  // tft.setRotation(1);
  // tft.fillScreen(TFT_BLACK);
  // tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // tft.setTextSize(2);
  // tft.drawString("ILI9225 OK", 10, 10, 2);
  
  if (!timer0.attachInterruptInterval(1000000, onTimer)) {
    // Timer start failed
    tft.drawString("Timer ERR", 10, 40, 2);
  }

}

void loop() {
  if (timerTicked) {
    timerTicked = false;
    snprintf(timerText, sizeof(timerText), "%08u", timerCount);
    display.DisplayText(timerText, 0, true);
    // tft.fillRect(10, 40, 220, 30, TFT_BLACK);
    // tft.drawString(String("Timer: ") + timerText, 10, 40, 2);
  }
}

