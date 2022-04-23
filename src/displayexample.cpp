#include <Arduino.h>
#include "display.h"
#include "pindefs.h"

/**
    Function declarations
*/
void rightButtonPressed();
void downButtonPressed();
void selectButtonPressed();
void handleButtonPress();

/**
   Display variables
   TODO: Move to display.h
   TODO: Add OLED variables
*/
uint8_t line = 0;
uint8_t frame = 0;
uint8_t displayTimer = millis();


/*
    IO variables
*/
volatile bool isRightButtonPressed = false;
volatile bool isDownButtonPressed = false;
volatile bool isSelectButtonPressed = false;
volatile uint32_t buttonPressTime = 0;


void setup() {
  // Input handling
  pinMode(RIGHTBUTTON, INPUT_PULLUP);
  pinMode(DOWNBUTTON, INPUT_PULLUP);
  pinMode(SELECTBUTTON, INPUT_PULLUP);
  // Attach interrupt functions
  attachInterrupt(digitalPinToInterrupt(RIGHTBUTTON), rightButtonPressed,
                  FALLING);
  attachInterrupt(digitalPinToInterrupt(DOWNBUTTON), downButtonPressed,
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
      settings.intervals[settings.displayUpdateIntervalIndex] * 1000) {
    displayTimer = millis();
    updateDisplay(oled, tft, data, settings, gps, tftState, line);
  }
}

//****************************************************************************
// * Input handling and interrupts
//****************************************************************************

void handleButtonPress() {
  // Check if the user switched to a new menu tab
  if (isRightButtonPressed) { // TODO: Rename this to right button
    isRightButtonPressed = false;
    substate += 1;
    Serial.println(substate);
    updateDisplay(oled, tft, data, settings, gps, tftState, line);
  }

  // Check if the user switched to a new menu item
  if (isDownButtonPressed) { // TODO: Rename this to left button
    isDownButtonPressed = false;
    substate -= ((substate == 0) ? 0 : 1);
    updateDisplay(oled, tft, data, settings, gps, tftState, line);
  }

  // Check if the user clicked on an item
  // if (isSelectButtonPressed) {
  //   isSelectButtonPressed = false;
  //   switch (line) {
  //     case DISPLAYRATE:  // Update rate changed
  //       settings.displayUpdateIntervalIndex =
  //           (settings.displayUpdateIntervalIndex + 1) %
  //           settings.intervals.size();
  //       break;
  //     case DATARATE:
  //       settings.dataUpdateIntervalIndex =
  //           (settings.dataUpdateIntervalIndex + 1) % settings.intervals.size();
  //       break;
  //     case TEMPERATUREUNIT:  // Temperature unit changed
  //       settings.temperatureUnit =
  //           (settings.temperatureUnit == FAHRENHEIT ? CELSIUS : FAHRENHEIT);
  //       break;
  //     default:
  //       break;
  //   }
  //   updateDisplay(oled, tft, data, settings, gps, frame, line);
  // }
}

void rightButtonPressed() {
  int timeNow = millis();
  if (timeNow > buttonPressTime + 250) {
    isRightButtonPressed = true;
    buttonPressTime = timeNow;
  }
}

void downButtonPressed() {
  int timeNow = millis();
  if (timeNow > buttonPressTime + 250) {
    isDownButtonPressed = true;
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