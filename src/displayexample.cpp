// #include <Arduino.h>

// #include "display.h"
// #include "pindefs.h"

// void setup() {
//   // Input handling
//   pinMode(RIGHTBUTTON, INPUT_PULLUP);
//   pinMode(LEFTBUTTON, INPUT_PULLUP);
//   pinMode(SELECTBUTTON, INPUT_PULLUP);
//   // Attach interrupt functions
//   attachInterrupt(digitalPinToInterrupt(RIGHTBUTTON), rightButtonPressed,
//                   FALLING);
//   attachInterrupt(digitalPinToInterrupt(LEFTBUTTON), leftbuttonPressed,
//                   FALLING);
//   attachInterrupt(digitalPinToInterrupt(SELECTBUTTON), selectButtonPressed,
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



