// #include <Arduino.h>

// #include "display.h"
// #include "pindefs.h"

// void setup() {
//   // Input handling
//   pinMode(RIGHT_BUTTON, INPUT_PULLUP);
//   pinMode(LEFT_BUTTON, INPUT_PULLUP);
//   pinMode(SELECT_BUTTON, INPUT_PULLUP);
//   // Attach interrupt functions
//   attachInterrupt(digitalPinToInterrupt(RIGHT_BUTTON), rightButtonPressed,
//                   FALLING);
//   attachInterrupt(digitalPinToInterrupt(LEFT_BUTTON), leftbuttonPressed,
//                   FALLING);
//   attachInterrupt(digitalPinToInterrupt(SELECT_BUTTON), selectButtonPressed,
//                   FALLING);
//   Serial.begin(9600);
//   // Wait for USB Serial
//   while (not Serial) {
//     yield();
//   }

//   // Start all devices
//   init_tft();
//   if (useOled) {
//     init_oled();
//   }
// }

// // *******************************************************************************
// // * Processing loop
// // *******************************************************************************
// void loop() {
//   // Periodic display update
//   handleButtonPress();
//   if (millis() - displayTimer >
//       1000) {
//     displayTimer = millis();
//     updateDisplay(oled, tft, data, settings, gps, tftState);
//   }
//   data.temperature += 1;
// }



