#ifndef B32C52E8_0A82_4B4F_BB96_D6D227CC2F94
#define B32C52E8_0A82_4B4F_BB96_D6D227CC2F94

#include <HardwareSerial.h>
#include <vector>

#include "constants.h"
#include "pindefs.h"
#include "settings.h"

float readWindSpeed();
float readTemperature(int units);

std::vector<float> readUV();



// Data from all sensors
// NOTE: UV won't actually be used with the OLED display
struct SensorData {
  float temperature;
  int humidity;
  float pressure;
  float windSpeed;
  std::vector<float> uv;
  float lat;
  float lon;
  int speed;
};

/**
   Sensor objects
*/
using namespace NeoGPS;
std::vector<int> uvPins = {UV1, UV2, UV3, UV4};
NMEAGPS gps;
gps_fix gpsData;
Adafruit_BME280 bme(BME_CS);
SensorData data;

/**
   Set up the GPS object

   INPUTS:
   - serial: The hardware serial port used by the GPS
   - rx: GPS RX pin
   - tx: GPS TX pin
*/
void init_gps(HardwareSerial serial, uint8_t rx, uint8_t tx) {
  serial.begin(9600, SERIAL_8N1, rx, tx);
  // By default, "16E TTL" and other U-blox GPS modules output RMC, VTG, CGA,
  // GSA, GSV, GLL messages once a second (these are standard NMEA messages)
  // Configure the GPS to only output what is needed, like just RMC as is shown
  // below by disabling all other default sentences.
  // gpsPort.println(F("$PUBX,40,RMC,0,0,0,0*47")); //RMC ON (Comented out to
  // leave RMC on...uncomment this line to disable RMC) delay(100);
  serial.println(F("$PUBX,40,VTG,0,0,0,0*5E"));  // VTG OFF
  delay(100);
  serial.println(F("$PUBX,40,GGA,0,0,0,0*5A"));  // GGA OFF
  delay(100);
  serial.println(F("$PUBX,40,GSA,0,0,0,0*4E"));  // GSA OFF
  delay(100);
  serial.println(F("$PUBX,40,GSV,0,0,0,0*59"));  // GSV OFF
  delay(100);
  serial.println(F("$PUBX,40,GLL,0,0,0,0*5C"));  // GLL OFF
  delay(100);
}

void updateData(SensorData& data, gps_fix gpsData, int temperatureUnit) {
  data.temperature = readTemperature(temperatureUnit);
  data.humidity = bme.readHumidity();
  data.pressure = bme.readPressure() / 100;
  data.windSpeed = readWindSpeed();
  // GPS data
  data.lat = gpsData.latitude();
  data.lon = gpsData.longitude();
  data.speed = gpsData.speed_mph();
  data.uv = readUV();
}

/**
   Probe the BME280 for temperature, converting to fahrenheit if necessary
*/
float readTemperature(int unit) {
  float temperature = bme.readTemperature();
  if (unit == FAHRENHEIT)
    temperature = (9.0 / 5.0) * temperature + 32;
  return temperature;
}

/**
   Get the wind speed
   TODO: Check these computations
*/
float readWindSpeed() {
  const float zeroWindAdjustment =  -0.85; // negative numbers yield smaller wind speeds and vice versa.
  float tmp_adc = analogRead(TMP);
  float rv_v = (analogRead(RV) / pow(2, 12));

  float zero_wind_adc = -0.0006 * ((float)tmp_adc * (float)tmp_adc) +
                        1.0727 * (float)tmp_adc + 47.172; //  13.0C  553  482.39

  float zero_wind_v = (zero_wind_adc / pow(2, 12)) - zeroWindAdjustment;

  // This from a regression from data in the form of
  // Vraw = V0 + b * WindSpeed ^ c
  // V0 is zero wind at a particular temperature
  // The constants b and c were determined by some Excel wrangling with the solver.
  return pow(((rv_v - zero_wind_v) / .2300) , 2.7265);
}

/**
   Read the UV index for each sensor defined in the uvPins vector
*/
std::vector<float> readUV() {
  std::vector<float> uvIndex(uvPins.size());
  for (int i = 0; i < uvPins.size(); i++) {
    uvIndex[i] = analogRead(uvPins[i]) * ADC_TO_VOLTAGE / 0.1;
  }
  return uvIndex;
}

#endif /* B32C52E8_0A82_4B4F_BB96_D6D227CC2F94 */
