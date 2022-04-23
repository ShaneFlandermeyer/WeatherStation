#ifndef B32C52E8_0A82_4B4F_BB96_D6D227CC2F94
#define B32C52E8_0A82_4B4F_BB96_D6D227CC2F94

#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>

#include <vector>

#include "constants.h"
#include "file.h"
#include "pindefs.h"
#include "settings.h"

float readWindSpeed();
float readWindDirection();
float readTemperature(int units);

std::vector<float> readUV();

// Data from all sensors
struct SensorData {
  float temperature;
  int humidity;
  float pressure;
  float windSpeed;
  float windDirection;
  std::vector<float> uv;
  float lat;
  float lon;
  float alt;
  int speed;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint32_t date;
};

/**
   Sensor objects
*/
std::vector<int> uvPins = {UV1, UV2, UV3, UV4, UV5};
TinyGPSPlus gps;
Adafruit_BME280 bme(BME_CS);
SensorData data;

void updateData(SensorData& data, TinyGPSPlus gps, int temperatureUnit) {
  // GPS data
  if (gps.time.isValid()) {
    data.minute = gps.time.minute();
    data.second = gps.time.second();
    data.date = gps.date.value();
  }
  if (gps.location.isValid()) {
    data.lat = gps.location.lat();
    data.lon = gps.location.lng();
    data.alt = gps.altitude.meters();
    data.speed = gps.speed.mph();
    data.hour = gps.time.hour();
  }
  data.temperature = readTemperature(temperatureUnit);
  data.humidity = bme.readHumidity();
  data.pressure = bme.readPressure() / 100;
  data.windSpeed = readWindSpeed();
  data.windDirection = readWindDirection();
  data.uv = readUV();
}

void writeData(fs::FS& fs, SensorData& data, const char* path) {
  // TODO: Add non-pcb sensor data to this operation
  File file = fs.open(path);
  // File does not exist...make data columns
  if (not file) {
    appendFile(
        fs, path,
        "Time, Date, Temperature, Pressure, Humidity, Wind speed, Latitude, "
        "Longitude\n");
  }
  String dataString = String(data.date) + "," + String(data.hour) + ":" +
                      String(data.minute) + ":" + String(data.second) +
                      " UTC," + String(data.temperature) + "," +
                      String(data.humidity) + "," + String(data.pressure) +
                      "," + String(data.windSpeed) + "," + String(data.lat) +
                      "," + String(data.lon) + "\n";
  appendFile(fs, path, dataString.c_str());
}

/**
   Probe the BME280 for temperature, converting to fahrenheit if necessary
*/
float readTemperature(int unit) {
  float temperature = bme.readTemperature();
  if (unit == FAHRENHEIT) temperature = (9.0 / 5.0) * temperature + 32;
  return temperature;
}

/**
   Get the wind speed
   TODO: Check these computations
*/
float readWindSpeed() {
  const float zeroWindAdjustment =
      -0.85;  // negative numbers yield smaller wind speeds and vice versa.
  float tmp_adc = analogRead(TMP);
  float rv_v = (analogRead(RV) / pow(2, 12));

  float zero_wind_adc = -0.0006 * ((float)tmp_adc * (float)tmp_adc) +
                        1.0727 * (float)tmp_adc + 47.172;  //  13.0C  553 482.39

  float zero_wind_v = (zero_wind_adc / pow(2, 12)) - zeroWindAdjustment;

  // This from a regression from data in the form of
  // Vraw = V0 + b * WindSpeed ^ c
  // V0 is zero wind at a particular temperature
  // The constants b and c were determined by some Excel wrangling with the
  // solver.
  return pow(((rv_v - zero_wind_v) / .2300), 2.7265);
}

float readWindDirection() {
  // Convert the voltage to an angle
  return analogRead(WINDDIRECTION) * ADC_TO_VOLTAGE / 5 * 359;
}

/**
   Read the UV index for each sensor defined in the uvPins vector
*/
std::vector<float> readUV() {
  std::vector<float> uvIndex(uvPins.size());
  for (int i = 0; i < uvPins.size(); i++) {
    uvIndex[i] = analogRead(uvPins[i]) * ADC_TO_VOLTAGE / 0.1;

    Serial.println(uvIndex[i]);
  }
  return uvIndex;
}

#endif /* B32C52E8_0A82_4B4F_BB96_D6D227CC2F94 */
