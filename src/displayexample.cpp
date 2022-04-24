#include <Arduino.h>

#include "display.h"
#include "pindefs.h"

/**
    Function declarations
*/
// void rightButtonPressed();
// void leftbuttonPressed();
// void selectButtonPressed();
// void handleButtonPress();

/**
   Display variables
   TODO: Move to display.h
   TODO: Add OLED variables
*/
// uint8_t line = 0;
// uint8_t frame = 0;


/*
    IO variables
*/
// volatile uint32_t buttonPressTime = 0;

void setup() {
  // Input handling
  pinMode(RIGHTBUTTON, INPUT_PULLUP);
  pinMode(LEFTBUTTON, INPUT_PULLUP);
  pinMode(SELECTBUTTON, INPUT_PULLUP);
  // Attach interrupt functions
  attachInterrupt(digitalPinToInterrupt(RIGHTBUTTON), rightButtonPressed,
                  FALLING);
  attachInterrupt(digitalPinToInterrupt(LEFTBUTTON), leftbuttonPressed,
                  FALLING);
  attachInterrupt(digitalPinToInterrupt(SELECTBUTTON), selectButtonPressed,
                  FALLING);
  Serial.begin(9600);
  // Wait for USB Serial
  while (not Serial) {
    yield();
  }

  // Start all devices
  init_tft();
  if (useOled) {
    init_oled();
  }
}

// *******************************************************************************
// * Processing loop
// *******************************************************************************
void loop() {
  // Periodic display update
  handleButtonPress();
  if (millis() - displayTimer >
      1000) {
    displayTimer = millis();
    updateDisplay(oled, tft, data, settings, gps, tftState);
  }
  data.temperature += 1;
}



