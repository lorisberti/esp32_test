/*
for pin configuration check:
MAX7219_7Segment/max7219.h  → CLK=32, CS=21, DIN=33
TFT_eSPI/User_setup.h       → MOSI=23, SCLK=18, CS=25, DC=27, RST=26, BL=4
*/

#include <Arduino.h>
#include "max7219.h"
#include <SPI.h>
#include <TFT_eSPI.h>
#include <ESP32TimerInterrupt.h>

// --- Screen layout (landscape: 220 x 176) ---
#define SCREEN_W       220
#define BOOT_LINES     11
#define LINE_H         10
#define COUNTER_LABEL_Y 117
#define COUNTER_NUM_Y   136

MAX7219 display;
TFT_eSPI tft = TFT_eSPI();

ESP32Timer timer0(0);
volatile bool     timerTicked = false;
volatile uint32_t timerCount  = 0;

bool IRAM_ATTR onTimer(void *param)
{
  (void) param;
  timerTicked = true;
  timerCount++;
  return true;
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(1);

  // --- Boot console ---
  int yPos = 5;
  const int delayMs = 300;

  auto printLine = [&](const String &s) {
    tft.drawString(s, 5, yPos, 1);
    yPos += LINE_H;
    delay(delayMs);
  };

  printLine("ESP32 Boot Console");
  printLine("========================");
  printLine("Chip Model: "    + String(ESP.getChipModel()));
  printLine("Chip Revision: " + String(ESP.getChipRevision()));
  printLine("Flash Size: "    + String(ESP.getFlashChipSize() / (1024*1024)) + " MB");
  printLine("PSRAM Size: "    + String(ESP.getPsramSize()    / (1024*1024)) + " MB");
  printLine("CPU Frequency: " + String(ESP.getCpuFreqMHz())  + " MHz");
  printLine("Free Heap: "     + String(ESP.getFreeHeap()     / 1024)        + " KB");
  printLine("Free PSRAM: "    + String(ESP.getFreePsram()    / (1024*1024)) + " MB");
  printLine("========================");
  printLine("System Ready!");

  Serial.println("=== ESP32 Board Information ===");
  Serial.printf("Chip: %s rev%d\n",   ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("Flash: %lu MB\n",    ESP.getFlashChipSize() / (1024*1024));
  Serial.printf("CPU: %d MHz\n",      ESP.getCpuFreqMHz());
  Serial.printf("Free heap: %lu KB\n",ESP.getFreeHeap() / 1024);
  Serial.println("==============================");

  // --- Counter area ---
  tft.drawFastHLine(0, COUNTER_LABEL_Y - 2, SCREEN_W, TFT_DARKGREY);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawCentreString("COUNTER", SCREEN_W / 2, COUNTER_LABEL_Y, 1);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawCentreString("0", SCREEN_W / 2, COUNTER_NUM_Y, 4);

  // --- MAX7219 ---
  display.Begin();
  display.MAX7219_SetBrightness(8);
  for (int i = 0; i < 8; i++) display.DisplayChar(i, '0', false);

  // --- Timer: 1 s interval ---
  if (!timer0.attachInterruptInterval(1000000, onTimer)) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("Timer Error!", 5, yPos, 1);
    Serial.println("ERROR: Timer failed to start");
  }
}

void loop()
{
  if (timerTicked) {
    timerTicked = false;

    Serial.printf("[TICK] count=%lu\n", timerCount);

    // Direct per-digit write: bypass DisplayText justify logic entirely
    uint32_t val = timerCount;
    for (int i = 0; i < 8; i++) {
      display.DisplayChar(i, '0' + (val % 10), false);
      val /= 10;
    }

    // Update TFT counter (large centred number)
    tft.fillRect(0, COUNTER_NUM_Y, SCREEN_W, 30, TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawCentreString(String(timerCount), SCREEN_W / 2, COUNTER_NUM_Y, 4);
  }
}


