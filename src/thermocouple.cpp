// #include <SPI.h>
// #include "Adafruit_MAX31855.h"
// #include <Adafruit_BME280.h>
// #include <constants.h>


// // Default connection is using software SPI, but comment and uncomment one of
// // the two examples below to switch between software SPI and hardware SPI:

// // Example creating a thermocouple instance with software SPI on any three
// // digital IO pins.
// #define MAXDO   19
// #define MAXCS   12 // 12 and 13
// #define MAXCS2  13 // 12 and 13
// #define BME_CS  14
// #define MAXCLK  18
// #define SDA     21
// #define SCL     22

// // initialize the Thermocouple
// Adafruit_MAX31855 solar(MAXCLK, MAXCS, MAXDO);
// Adafruit_MAX31855 terrestrial(MAXCLK, MAXCS2, MAXDO);

// double readRefTemp() {
//   // TODO: Read this from analog sensor
//   return 275;
// }

// double readSolarRadiation() {
//   double tc_temp = 0;
//   for (int i = 0; i < 10; i++) {
//     tc_temp += solar.readCelsius() + 273.15;
//   }
//   tc_temp /= 10;
//   Serial.println("Solar Temp = " + String(tc_temp) + " K");
//   double tc = STEFAN_BOLTZMANN*pow(tc_temp, 4);
//   double ref = STEFAN_BOLTZMANN*pow(readRefTemp(),4);
//   double rad = abs(ref-tc);
//   Serial.println("Solar Rad = " + String(rad) +"W/m^2");
//   return rad;
  
// }

// double readTerrestrialRadiation() {
//   double tc_temp = 0;
//   for (int i = 0; i < 10; i++) {
//     tc_temp += terrestrial.readCelsius() + 273.15;
//   }
//   tc_temp /= 10;
//   Serial.println("Terrestrial Temp = " + String(tc_temp) + " K");
//   double tc = STEFAN_BOLTZMANN*pow(tc_temp,4);
//   double ref = STEFAN_BOLTZMANN*pow(readRefTemp(),4);
//   double rad = abs(ref-tc);
//   Serial.println("Terrestrial Rad = " + String(rad) +"W/m^2");
//   return rad;
// }

// void setup() {
//   Serial.begin(9600);

//   while (!Serial) delay(1); // wait for Serial on L

//   Serial.println("MAX31855 test");
//   // wait for MAX chip to stabilize
//   delay(500);
//   Serial.print("Initializing sensor...");
  
//   Serial.println("DONE.");
//   // bme.begin();
//   if (!solar.begin()) {
//     Serial.println("ERROR.");
//     while (1) delay(10);
//   }
//   if (!terrestrial.begin()) {
//     Serial.println("ERROR.");
//     while (1) delay(10);
//   }
// }

// void loop() {
//   // basic readout test, just print the current temp
  
//    Serial.print("Terrestrial = ");
//    Serial.println(readTerrestrialRadiation());
//    Serial.print("Solar = ");
//    Serial.println(readSolarRadiation());
//    delay(1000);
//   // Serial.println(bme.readTemperature());
// }