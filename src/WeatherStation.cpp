#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include <NMEAGPS.h>
#include <SPI.h>
#include <Wire.h>

#include <sstream>
#include <vector>

#include "bluetooth.h"
#include "display.h"
#include "pindefs.h"
#include "sensors.h"


// Configure GPS serial interface to use UART2
// HardwareSerial has to be used instead of Serial2 since the RX port for UART2
// also the reset pin for the OLED display (because heltec hates us >:( )
HardwareSerial GPSSerial(2);

/**
    Function declarations
*/
void rightButtonPressed();
void downButtonPressed();
void selectButtonPressed();
void handleButtonPress();

/**
   OLED display variables
*/
uint8_t frame = 0;
uint8_t line = 0;
uint8_t numFrames = 3;
uint8_t numSettings = 3;
uint32_t displayTimer = millis();
uint32_t dataTimer = millis();

/*
    IO variables
*/
volatile bool isRightButtonPressed = false;
volatile bool isDownButtonPressed = false;
volatile bool isSelectButtonPressed = false;
volatile uint32_t buttonPressTime = 0;

// *******************************************************************************
// * Setup functions
//   *******************************************************************************

bool deviceConnected = false;
class WeatherStationCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) { deviceConnected = true; };
  void onDisconnect(BLEServer *pServer) { deviceConnected = false; }
};

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
  while (!Serial) {
    yield();
  }

  // Start all devices
  init_gps(GPSSerial, GPS_RX, GPS_TX);
  init_display();
  bme.begin();

  BLEDevice::init("Portable Weather Station");
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new WeatherStationCallbacks());
  BLEService *service = server->createService(SERVICE_UUID);

  // Temperature
  service->addCharacteristic(&temperatureCharacteristic);
  service->addCharacteristic(&humidityCharacteristic);
  service->addCharacteristic(&pressureCharacteristic);
  service->addCharacteristic(&windspeedCharacteristic);

  // Start the service
  service->start();

  // Start advertising
  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);
  advertising->setMinPreferred(
      0x06);  // functions that help with iPhone connections issue
  advertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

// *******************************************************************************
// * Processing loop
// *******************************************************************************
void loop() {
  // Read GPS data
  while (gps.available(GPSSerial)) {
    // Save the gps data
    gpsData = gps.read();
  }
  handleButtonPress();

  // Periodic data update
  if (millis() - dataTimer >
      settings.intervals[settings.dataUpdateIntervalIndex] * 1000) {
    dataTimer = millis();
    updateData(data, gpsData, settings.temperatureUnit);

     // Send the data over bluetooth low energy
    if (deviceConnected) {
      bleSendTemperature(data,settings);
      bleSendHumidity(data);
      bleSendPressure(data);
      bleSendWindSpeed(data);
    }
  }

  // Periodic display update
  if (millis() - displayTimer >
      settings.intervals[settings.displayUpdateIntervalIndex] * 1000) {
    displayTimer = millis();
    updateDisplay(display, data, settings, gpsData, frame, line);
  }
}

// ****************************************************************************
// * Input handling and interrupts
// ****************************************************************************

void handleButtonPress() {
  // Check if the user switched to a new menu tab
  if (isRightButtonPressed) {
    isRightButtonPressed = false;
    frame = (frame + 1) % numFrames;
    updateDisplay(display, data, settings, gpsData, frame, line);
  }

  // Check if the user switched to a new menu item
  if (isDownButtonPressed) {
    isDownButtonPressed = false;
    line = (line + 1) % numSettings;
    updateDisplay(display, data, settings, gpsData, frame, line);
  }

  // Check if the user clicked on an item
  if (isSelectButtonPressed) {
    isSelectButtonPressed = false;
    switch (line) {
      case DISPLAYRATE:  // Update rate changed
        settings.displayUpdateIntervalIndex =
            (settings.displayUpdateIntervalIndex + 1) %
            settings.intervals.size();
        break;
      case DATARATE:
        settings.dataUpdateIntervalIndex =
            (settings.dataUpdateIntervalIndex + 1) % settings.intervals.size();
        break;
      case TEMPERATUREUNIT:  // Temperature unit changed
        settings.temperatureUnit =
            (settings.temperatureUnit == FAHRENHEIT ? CELSIUS : FAHRENHEIT);
        break;
      default:
        break;
    }
    updateDisplay(display, data, settings, gpsData, frame, line);
  }
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