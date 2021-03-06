#ifndef F6837E0A_A91C_4BFE_9070_93D920E5CE82
#define F6837E0A_A91C_4BFE_9070_93D920E5CE82

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "sensors.h"
#include "settings.h"

#define ENV_SENSING_UUID "181A"
#define TEMPERATURE_UUID "2A1C"
#define WIND_SPEED_UUID "2A72"
#define WIND_DIRECTION_UUID "2A73"
#define HUMIDITY_UUID "2A6F"
#define PRESSURE_UUID "2A6D"

#define LOC_NAV_UUID "1819"
#define LATITUDE_UUID "2AAE"
#define LOC_SPEED_UUID "2A67"


// BLE characteristics
BLECharacteristic temperatureCharacteristic(
    TEMPERATURE_UUID,
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ);
BLECharacteristic humidityCharacteristic(HUMIDITY_UUID,
                                         BLECharacteristic::PROPERTY_NOTIFY |
                                             BLECharacteristic::PROPERTY_READ);
BLECharacteristic pressureCharacteristic(PRESSURE_UUID,
                                         BLECharacteristic::PROPERTY_NOTIFY |
                                             BLECharacteristic::PROPERTY_READ);
BLECharacteristic windspeedCharacteristic(WIND_SPEED_UUID,
                                          BLECharacteristic::PROPERTY_NOTIFY |
                                              BLECharacteristic::PROPERTY_READ);
BLECharacteristic windDirectionCharacteristic(WIND_DIRECTION_UUID,
                                          BLECharacteristic::PROPERTY_NOTIFY |
                                              BLECharacteristic::PROPERTY_READ);
BLECharacteristic locationCharacteristic(LATITUDE_UUID,
                                          BLECharacteristic::PROPERTY_NOTIFY |
                                              BLECharacteristic::PROPERTY_READ);


void bleSendTemperature(const SensorData& data, const Settings& settings) {
  // Send the temperature
  int32_t temperature = data.temperature;
  uint8_t buf[6];
  if (settings.temperatureUnit == CELSIUS)
    buf[0] = 0;
  else
    buf[0] = 0x01;
  memcpy(&buf[1], &temperature, 4);
  temperatureCharacteristic.setValue(buf, sizeof(buf));
  temperatureCharacteristic.notify();
}

void bleSendHumidity(const SensorData& data) {
  uint16_t humidity = round(data.humidity * 100.0);
  humidityCharacteristic.setValue((uint8_t*)&humidity, 2);
  humidityCharacteristic.notify();
}

void bleSendPressure(const SensorData& data) {
  uint32_t pressure = round(data.pressure * 10.0);
  pressureCharacteristic.setValue((uint8_t*)&pressure, 4);
  pressureCharacteristic.notify();
}

void bleSendWindSpeed(const SensorData& data) {
  uint16_t windspeed = round(data.windSpeed * 100.0);
  windspeedCharacteristic.setValue((uint8_t*)&windspeed, 2);
  windspeedCharacteristic.notify();
}

void bleSendWindDirection(const SensorData &data) {
  uint16_t windDirection = round(data.windDirection * 100.0);
  windDirectionCharacteristic.setValue((uint8_t*)&windDirection, 2);
  windDirectionCharacteristic.notify();
}

void bleSendGPSData(const SensorData &data) {
  int lat = round(50 * 10000000.0);
  locationCharacteristic.setValue((uint8_t*)&lat, 4);
  locationCharacteristic.notify();
  // int32_t lat = 35;
  // int32_t lon = 35;
  // uint8_t buf[10];
  // buf[0] = 0x04;
  // buf[1] = 0x00;
  // memcpy(&buf[2], &lat, 4);
  // memcpy(&buf[6], &lon, 4);
  // locationCharacteristic.setValue(buf, sizeof(buf));
  // locationCharacteristic.notify();
}

#endif /* F6837E0A_A91C_4BFE_9070_93D920E5CE82 */
