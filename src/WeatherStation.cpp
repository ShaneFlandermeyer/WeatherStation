// // #include <Adafruit_BME280.h>
// // #include <Adafruit_Sensor.h>
// #include <Arduino.h>
// #include <HardwareSerial.h>
// // #include <SPI.h>
// // #include <TinyGPS++.h>
// // #include <Wire.h>
// #include <SD.h>

// #include <sstream>
// #include <vector>

// #include "bluetooth.h"
// #include "display.h"
// #include "file.h"
// #include "pindefs.h"
// #include "sensors.h"

// // Configure GPS serial interface to use UART2
// // HardwareSerial has to be used instead of Serial2 since the RX port for UART2
// // also the reset pin for the OLED display (because heltec hates us >:( )
// HardwareSerial SerialGPS(2);

// /**
//     Function declarations
// */
// void rightButtonPressed();
// void leftbuttonPressed();
// void selectButtonPressed();
// void handleButtonPress();

// /**
//    OLED display variables
// */
// uint8_t frame = 0;
// uint8_t line = 0;
// uint8_t numFrames = 2;
// uint8_t numSettings = 3;
// uint32_t displayTimer = millis();
// uint32_t dataTimer = millis();
// uint32_t frameTimer = millis();

// /*
//     IO variables
// */
// volatile bool isRightButtonPressed = false;
// volatile bool isLeftButtonPressed = false;
// volatile bool isSelectButtonPressed = false;
// volatile uint32_t buttonPressTime = 0;

// // *******************************************************************************
// // * Setup functions
// //   *******************************************************************************

// bool deviceConnected = false;
// class WeatherStationCallbacks : public BLEServerCallbacks {
//   void onConnect(BLEServer *pServer) { deviceConnected = true; };
//   void onDisconnect(BLEServer *pServer) { deviceConnected = false; }
// };

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
//   bme.begin();
//   BLEDevice::init("Portable Weather Station");
//   BLEServer *server = BLEDevice::createServer();
//   server->setCallbacks(new WeatherStationCallbacks());
//   BLEService *env_service = server->createService(ENV_SENSING_UUID);
//   BLEService *loc_service = server->createService(LOC_NAV_UUID);

//   // Environmental sensing
//   env_service->addCharacteristic(&temperatureCharacteristic);
//   env_service->addCharacteristic(&humidityCharacteristic);
//   env_service->addCharacteristic(&pressureCharacteristic);
//   env_service->addCharacteristic(&windspeedCharacteristic);
//   env_service->addCharacteristic(&windDirectionCharacteristic);

//   // Location and navigation
//   loc_service->addCharacteristic(&locationCharacteristic);

//   // Start the service
//   env_service->start();
//   loc_service->start();

//   // Start advertising
//   BLEAdvertising *advertising = BLEDevice::getAdvertising();
//   advertising->addServiceUUID(ENV_SENSING_UUID);
//   advertising->addServiceUUID(LOC_NAV_UUID);
//   advertising->setScanResponse(true);
//   advertising->setMinPreferred(
//       0x06);  // functions that help with iPhone connections issue
//   advertising->setMinPreferred(0x12);
//   BLEDevice::startAdvertising();

//   SerialGPS.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

//   // Initialize the SD Card
//   if (not SD.begin()) {
//     Serial.println("No SD card attached");
//   }
// }

// // *******************************************************************************
// // * Processing loop
// // *******************************************************************************
// void loop() {
//   while (SerialGPS.available() > 0) {
//     char c = SerialGPS.read();
//     Serial.print(c);
//     gps.encode(c);
//   }
//   handleButtonPress();

//   // Periodic data update
//   if (millis() - dataTimer >
//       settings.intervals[settings.dataUpdateIntervalIndex] * 1000) {
//     dataTimer = millis();
//     updateData(data, gps, settings.temperatureUnit);
//     writeData(SD, data, "/data.csv");
//     // Send the data over bluetooth low energy
//     if (deviceConnected) {
//       bleSendTemperature(data, settings);
//       bleSendHumidity(data);
//       bleSendPressure(data);
//       bleSendWindSpeed(data);
//       bleSendWindDirection(data);
//       bleSendGPSData(data);
//     }
//   }

//   // Periodic display update
//   if (millis() - displayTimer >
//       settings.intervals[settings.displayUpdateIntervalIndex] * 1000) {
//     displayTimer = millis();
//     updateDisplay(oled, tft, data, settings, gps, frame, line);
//   }

//   if (millis() - frameTimer > 5000) {
//     frameTimer = millis();
//     frame = (frame + 1) % numFrames;
//     updateDisplay(oled, tft, data, settings, gps, frame, line);
//   }
// }

// //****************************************************************************
// // * Input handling and interrupts
// //****************************************************************************

// void handleButtonPress() {
//   // Check if the user switched to a new menu tab
//   if (isRightButtonPressed) {
//     isRightButtonPressed = false;
//     frame = (frame + 1) % numFrames;
//     updateDisplay(oled, tft, data, settings, gps, frame, line);
//   }

//   // Check if the user switched to a new menu item
//   if (isLeftButtonPressed) {
//     isLeftButtonPressed = false;
//     line = (line + 1) % numSettings;
//     updateDisplay(oled, tft, data, settings, gps, frame, line);
//   }

//   // Check if the user clicked on an item
//   if (isSelectButtonPressed) {
//     isSelectButtonPressed = false;
//     switch (line) {
//       case DISPLAYRATE:  // Update rate changed
//         settings.displayUpdateIntervalIndex =
//             (settings.displayUpdateIntervalIndex + 1) %
//             settings.intervals.size();
//         break;
//       case DATARATE:
//         settings.dataUpdateIntervalIndex =
//             (settings.dataUpdateIntervalIndex + 1) % settings.intervals.size();
//         break;
//       case TEMPERATUREUNIT:  // Temperature unit changed
//         settings.temperatureUnit =
//             (settings.temperatureUnit == FAHRENHEIT ? CELSIUS : FAHRENHEIT);
//         break;
//       default:
//         break;
//     }
//     updateDisplay(oled, tft, data, settings, gps, frame, line);
//   }
// }

// void rightButtonPressed() {
//   int timeNow = millis();
//   if (timeNow > buttonPressTime + 250) {
//     isRightButtonPressed = true;
//     buttonPressTime = timeNow;
//   }
// }

// void leftbuttonPressed() {
//   int timeNow = millis();
//   if (timeNow > buttonPressTime + 250) {
//     isLeftButtonPressed = true;
//     buttonPressTime = timeNow;
//   }
// }

// void selectButtonPressed() {
//   int timeNow = millis();
//   if (timeNow > buttonPressTime + 250) {
//     isSelectButtonPressed = true;
//     buttonPressTime = timeNow;
//   }
// }