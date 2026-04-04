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

void setup() 
{
  Serial.begin(115200);
  delay(1000); // Give serial time to initialize

  // Initialize TFT for console-style boot display
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK); // Classic console green text
  tft.setTextSize(1); // Small font for console look

  // Simulate boot console output
  int yPos = 5;
  const int lineHeight = 10;
  const int delayMs = 300;

  tft.drawString("ESP32 Boot Console", 5, yPos, 1);
  yPos += lineHeight;
  delay(delayMs);

  tft.drawString("========================", 5, yPos, 1);
  yPos += lineHeight;
  delay(delayMs);

  String chipModel = "Chip Model: " + String(ESP.getChipModel());
  tft.drawString(chipModel, 5, yPos, 1);
  yPos += lineHeight;
  delay(delayMs);

  String chipRev = "Chip Revision: " + String(ESP.getChipRevision());
  tft.drawString(chipRev, 5, yPos, 1);
  yPos += lineHeight;
  delay(delayMs);

  String flashSize = "Flash Size: " + String(ESP.getFlashChipSize() / (1024 * 1024)) + " MB";
  tft.drawString(flashSize, 5, yPos, 1);
  yPos += lineHeight;
  delay(delayMs);

  String psramSize = "PSRAM Size: " + String(ESP.getPsramSize() / (1024 * 1024)) + " MB";
  tft.drawString(psramSize, 5, yPos, 1);
  yPos += lineHeight;
  delay(delayMs);

  String cpuFreq = "CPU Frequency: " + String(ESP.getCpuFreqMHz()) + " MHz";
  tft.drawString(cpuFreq, 5, yPos, 1);
  yPos += lineHeight;
  delay(delayMs);

  String freeHeap = "Free Heap: " + String(ESP.getFreeHeap() / 1024) + " KB";
  tft.drawString(freeHeap, 5, yPos, 1);
  yPos += lineHeight;
  delay(delayMs);

  String freePsram = "Free PSRAM: " + String(ESP.getFreePsram() / (1024 * 1024)) + " MB";
  tft.drawString(freePsram, 5, yPos, 1);
  yPos += lineHeight;
  delay(delayMs);

  tft.drawString("========================", 5, yPos, 1);
  yPos += lineHeight;
  delay(delayMs);

  tft.drawString("System Ready!", 5, yPos, 1);

  // Keep Serial output for debugging
  Serial.println("=== ESP32 Board Information ===");
  Serial.print("Chip Model: ");
  Serial.println(ESP.getChipModel());
  Serial.print("Chip Revision: ");
  Serial.println(ESP.getChipRevision());
  Serial.print("Flash Chip Size: ");
  Serial.println(ESP.getFlashChipSize() / (1024 * 1024));
  Serial.println("MB");
  Serial.print("PSRAM Size: ");
  Serial.println(ESP.getPsramSize() / (1024 * 1024));
  Serial.println("MB");
  Serial.print("CPU Frequency: ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" MHz");
  Serial.print("Free Heap: ");
  Serial.print(ESP.getFreeHeap() / 1024);
  Serial.println(" KB");
  Serial.print("Free PSRAM: ");
  Serial.print(ESP.getFreePsram() / (1024 * 1024));
  Serial.println(" MB");
  Serial.println("==============================");

  // Initialize MAX7219 display
  display.Begin();
  display.Clear();
  display.MAX7219_SetBrightness(8);
  display.DisplayText(timerText, 0, true);

  // Setup timer
  if (!timer0.attachInterruptInterval(1000000, onTimer)) {
    // Timer start failed - display on TFT
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("Timer Error!", 5, yPos + lineHeight, 1);
  }

}

void loop() 
{
  if (timerTicked) {
    timerTicked = false;
    snprintf(timerText, sizeof(timerText), "%08u", timerCount);
    // display.DisplayText(timerText, 0, true);
    // tft.fillRect(10, 40, 220, 30, TFT_BLACK);
    // tft.drawString(String("Timer: ") + timerText, 10, 40, 2);
  }
}

