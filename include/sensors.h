#ifndef B32C52E8_0A82_4B4F_BB96_D6D227CC2F94
#define B32C52E8_0A82_4B4F_BB96_D6D227CC2F94

#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <Adafruit_MAX31855.h>

#include <vector>

#include "constants.h"
#include "file.h"
#include "pindefs.h"
#include "settings.h"

float readWindSpeed();
float readWindDirection();
float readTemperature(int units);
double readSolarRadiation();
double readTerrestrialRadiation();

std::vector<float> readUV();

// Data from all sensors
struct SensorData {
  float temperature;
  int humidity;
  float pressure;
  float windSpeed;
  float windDirection;
  std::vector<float> uv;
  float solarRadiation;
  float terrestrialRadiation;
  double lat;
  double lon;
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
// TODO: Define thermocouple SPI pins
Adafruit_MAX31855 solar(SCK,SOLAR_THERMOCOUPLE, MISO);
Adafruit_MAX31855 terrestrial(SCK, TERRESTRIAL_THERMOCOUPLE, MISO);
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
  data.solarRadiation = readSolarRadiation();
  data.terrestrialRadiation = readTerrestrialRadiation();
}

void writeData(fs::FS& fs, SensorData& data, const char* path) {
  // TODO: Add non-pcb sensor data to this operation
  File file = fs.open(path);

  // CSV column headers
  String headerStr = "Date, Time, Temperature, Pressure, Humidity, Wind speed, Wind direction, UV1, UV2, UV3, UV4, UV5, Latitude, Longitude, Altitude, Speed";
  // If the first lins is not the CSV column headers, overwrite the garbage data
  String line = file.readStringUntil('\n');
  if (not line.equals(headerStr)) {
    String firstLine = headerStr + "\n";
    writeFile(fs,path,firstLine.c_str());

  }
  // Sensor data
  String dataString = String(data.date) + "," + String(data.hour) + ":" +
                      String(data.minute) + ":" + String(data.second) +
                      " UTC," + String(data.temperature) + "," +
                      String(data.pressure) + "," + String(data.humidity) +
                      "," + String(data.windSpeed) + "," + 
                      String(data.windDirection) + ",";
  // UV sensor data
  for (auto uv : data.uv) {
    dataString += String(uv) + ",";
  }
  // GPS data
  dataString += String(data.lat,6) + "," + String(data.lon,6) + "," + String(data.alt) + "," + String(data.speed) + "\n";
  appendFile(fs, path, dataString.c_str());
}

/**
   Probe the BME280 for temperature, converting to fahrenheit if necessary
*/
float readTemperature(int unit) {
  float temperature = bme.readTemperature();
  if (unit == FAHRENHEIT) temperature = (9.0 / 5.0) * temperature + 32;
  Serial.println("Temperature: " + String(temperature) + " " +
                 (unit == FAHRENHEIT ? "F" : "C")); 
  return temperature;
}

/**
   Get the wind speed
   TODO: Check these computations
*/
float readWindSpeed() {
  analogReadResolution(10);
  const float zeroWindAdjustment =
      -0.11;  // negative numbers yield smaller wind speeds and vice versa.
  float tmp_adc = analogRead(TMP);
  float rv_v = analogRead(RV) * 0.0048828125;

  

  float zero_wind_adc = -0.0006 * ((float)tmp_adc * (float)tmp_adc) +
                        1.0727 * (float)tmp_adc + 47.172;  //  13.0C  553 482.39

  float zero_wind_v = (zero_wind_adc * 0.0048828125) - zeroWindAdjustment;

  // This from a regression from data in the form of
  // Vraw = V0 + b * WindSpeed ^ c
  // V0 is zero wind at a particular temperature
  // The constants b and c were determined by some Excel wrangling with the
  // solver.
  analogReadResolution(12);
  float windSpeed = pow(((rv_v - zero_wind_v) / .2300), 2.7265);
  Serial.println("Wind speed: "  + String(windSpeed) + " mph");
  return windSpeed;
}

float readWindDirection() {
  // Convert the voltage to an angle
  float windDirection = analogRead(WIND_DIRECTION) * ADC_TO_VOLTAGE / 5 * 359;
  Serial.println("Wind direction: " + String(windDirection) + " deg");
  return windDirection;
}

/**
   Read the UV index for each sensor defined in the uvPins vector
*/
std::vector<float> readUV() {
  std::vector<float> uvIndex(uvPins.size());
  for (int i = 0; i < uvPins.size(); i++) {
    uvIndex[i] = analogRead(uvPins[i]) * ADC_TO_VOLTAGE / 0.1;
    Serial.println(String(uvIndex[i]));
  }
  return uvIndex;
}

/*
 * Convert the temperature difference between thermocouples to a radiation value
 * in W/m^2 using the Stefan-Boltzmann equation for the SOLAR radiometer
 */
double readSolarRadiation() {
  double tc = STEFAN_BOLTZMANN*pow(solar.readCelsius() + 273.15,4);
  double ref = STEFAN_BOLTZMANN*pow(bme.readTemperature() + 273.15,4);
  return abs(ref-tc);
  
}

/*
 * Convert the temperature difference between thermocouples to a radiation value
 * in W/m^2 using the Stefan-Boltzmann equation for the TERRESTRIAL radiometer
 */
double readTerrestrialRadiation() {
  double tc = STEFAN_BOLTZMANN*pow(terrestrial.readCelsius() + 273.15,4);
  double ref = STEFAN_BOLTZMANN*pow(bme.readTemperature() + 273.15,4);
  return abs(ref-tc);
}

#endif /* B32C52E8_0A82_4B4F_BB96_D6D227CC2F94 */
