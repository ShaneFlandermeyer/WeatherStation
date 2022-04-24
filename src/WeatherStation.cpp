#include <Arduino.h>
#include <HardwareSerial.h>
#include <SD.h>

#include <sstream>
#include <vector>

#include "bluetooth.h"
#include "display.h"
#include "file.h"
#include "pindefs.h"
#include "sensors.h"

// Configure GPS serial interface to use UART2
// HardwareSerial has to be used instead of Serial2 since the RX port for UART2
// also the reset pin for the OLED display (because heltec hates us >:( )
HardwareSerial SerialGPS(2);
uint64_t dataTimer = millis();

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
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(SELECT_BUTTON, INPUT_PULLUP);
  pinMode(TFT_CS, OUTPUT);
  // Attach interrupt functions
  attachInterrupt(digitalPinToInterrupt(RIGHT_BUTTON), rightButtonPress,
                  FALLING);
  attachInterrupt(digitalPinToInterrupt(LEFT_BUTTON), leftbuttonPress, FALLING);
  attachInterrupt(digitalPinToInterrupt(SELECT_BUTTON), selectButtonPress,
                  FALLING);
  Serial.begin(9600);
  // Wait for USB Serial
  while (not Serial) {
    yield();
  }

  // Start all devices
  init_tft();
  init_oled();
  bme.begin();
  BLEDevice::init("Portable Weather Station");
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new WeatherStationCallbacks());
  BLEService *env_service = server->createService(ENV_SENSING_UUID);
  BLEService *loc_service = server->createService(LOC_NAV_UUID);

  // Environmental sensing
  env_service->addCharacteristic(&temperatureCharacteristic);
  env_service->addCharacteristic(&humidityCharacteristic);
  env_service->addCharacteristic(&pressureCharacteristic);
  env_service->addCharacteristic(&windspeedCharacteristic);
  env_service->addCharacteristic(&windDirectionCharacteristic);

  // Location and navigation
  loc_service->addCharacteristic(&locationCharacteristic);

  // Start the service
  env_service->start();
  loc_service->start();

  // Start advertising
  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(ENV_SENSING_UUID);
  advertising->addServiceUUID(LOC_NAV_UUID);
  advertising->setScanResponse(true);
  advertising->setMinPreferred(
      0x06);  // functions that help with iPhone connections issue
  advertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  SerialGPS.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

  // Initialize the SD Card
  if (not SD.begin()) {
    Serial.println("No SD card attached");
  }
}

// *******************************************************************************
// * Processing loop
// *******************************************************************************
void loop() {
  while (SerialGPS.available() > 0) {
    char c = SerialGPS.read();
    Serial.print(c);
    gps.encode(c);
  }
  handleButtonPress();

  // Periodic data update
  if (millis() - dataTimer >
      settings.intervals[settings.dataUpdateIntervalIndex] * 1000) {
    dataTimer = millis();
    updateData(data, gps, settings.temperatureUnit);
    writeData(SD, data, "/data.csv");
    // Send the data over bluetooth low energy
    if (deviceConnected) {
      bleSendTemperature(data, settings);
      bleSendHumidity(data);
      bleSendPressure(data);
      bleSendWindSpeed(data);
      bleSendWindDirection(data);
      bleSendGPSData(data);
    }
  }

  // Periodic display update
  if (millis() - displayTimer >
      settings.intervals[settings.displayUpdateIntervalIndex] * 1000) {
    displayTimer = millis();
    updateDisplay(oled, tft, data, settings, gps);
  }

  if (settings.useOled and
      millis() - oledFrameTimer >
          settings.intervals[settings.oledScrollRateIntervalIndex] * 1000) {
    oledFrameTimer = millis();
    oledState = (oledState + 1) % NUM_OLED_FRAMES;
    updateDisplay(oled, tft, data, settings, gps);
  }
}