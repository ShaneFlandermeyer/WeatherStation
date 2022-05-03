// // /***************************************************
// //   This is an example for the Adafruit Thermocouple Sensor w/MAX31855K

// //   Designed specifically to work with the Adafruit Thermocouple Sensor
// //   ----> https://www.adafruit.com/products/269

// //   These displays use SPI to communicate, 3 pins are required to
// //   interface
// //   Adafruit invests time and resources providing this open source code,
// //   please support Adafruit and open-source hardware by purchasing
// //   products from Adafruit!

// //   Written by Limor Fried/Ladyada for Adafruit Industries.
// //   BSD license, all text above must be included in any redistribution
// //  ****************************************************/

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
//   double tc = STEFAN_BOLTZMANN*pow(solar.readCelsius() + 273.15,4);
//   double ref = STEFAN_BOLTZMANN*pow(readRefTemp(),4);
//   return abs(ref-tc);
  
// }

// double readTerrestrialRadiation() {
//   double tc = STEFAN_BOLTZMANN*pow(terrestrial.readCelsius() + 273.15,4);
//   double ref = STEFAN_BOLTZMANN*pow(readRefTemp(),4);
//   return abs(ref-tc);
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