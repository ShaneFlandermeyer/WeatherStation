#include <Arduino.h>

#include "display.h"
#include "pindefs.h"

/**
    Function declarations
*/
void rightButtonPressed();
void leftbuttonPressed();
void selectButtonPressed();
void handleButtonPress();

/**
   Display variables
   TODO: Move to display.h
   TODO: Add OLED variables
*/
// uint8_t line = 0;
// uint8_t frame = 0;
uint64_t displayTimer = millis();

/*
    IO variables
*/
volatile uint32_t buttonPressTime = 0;

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

//****************************************************************************
// * Input handling and interrupts
//****************************************************************************

void handleButtonPress() {
  // Check if the user switched to a new menu tab
  if (isRightButtonPressed) {
    isRightButtonPressed = false;
    substate += 1;
    Serial.println(substate);
    updateDisplay(oled, tft, data, settings, gps, tftState);
  }

  // Check if the user switched to a new menu item
  if (isLeftButtonPressed) {
    isLeftButtonPressed = false;
    substate -= ((substate == 0) ? 0 : 1);
    updateDisplay(oled, tft, data, settings, gps, tftState);
  }

  // Check if the user clicked on an item
  if (isSelectButtonPressed) {
    updateDisplayParams();
    isSelectButtonPressed = false;
    // TODO: Remove this and replace the above updateDisplay with a parameter handler
    updateDisplay(oled, tft, data, settings, gps, tftState);
  }
}

void rightButtonPressed() {
  int timeNow = millis();
  if (timeNow > buttonPressTime + 250) {
    isRightButtonPressed = true;
    buttonPressTime = timeNow;
  }
}

void leftbuttonPressed() {
  int timeNow = millis();
  if (timeNow > buttonPressTime + 250) {
    isLeftButtonPressed = true;
    buttonPressTime = timeNow;
  }
}

void selectButtonPressed() {
  int timeNow = millis();
  if (timeNow > buttonPressTime + 250) {
    isSelectButtonPressed = true;
    buttonPressTime = timeNow;
  }
}