// #include <SPI.h>
// #include "Adafruit_MAX31855.h"
// #include "DHT.h"


// // Default connection is using software SPI, but comment and uncomment one of
// // the two examples below to switch between software SPI and hardware SPI:

// // Example creating a thermocouple instance with software SPI on any three
// // digital IO pins.
// #define MAXDO   19
// #define MAXCS   12 3
// #define MAXCS2  13
// #define DHTPIN 14 
// #define DHTTYPE DHT11
// #define MAXCLK  18
// #define SDA     21
// #define SCL     22
// #define STEFAN_BOLTZMANN 5.6070374e-8

// // initialize the Thermocouple
// Adafruit_MAX31855 solar(MAXCLK, MAXCS, MAXDO);
// Adafruit_MAX31855 terrestrial(MAXCLK, MAXCS2, MAXDO);
// DHT dht(DHTPIN, DHTTYPE);


// double readRefTemp() {
//   return dht.readTemperature() + 273.15;
  
// }

// double readSolarRadiation() {
//   double tc_temp = 0;
//   double ref_temp = 0;
//   for (int i = 0; i < 10; i++) {
//     tc_temp += solar.readCelsius() + 273.15;
//     ref_temp += readRefTemp();
//   }
//   tc_temp /= 10;
//   ref_temp /= 10;
//   double tc = STEFAN_BOLTZMANN*pow(tc_temp, 4);
//   double ref = STEFAN_BOLTZMANN*pow(readRefTemp(),4);
//   double rad = abs(ref-tc);
//   Serial.println("Reference Temp = " + String(ref_temp) + " K");
//   Serial.println("Solar Temp = " + String(tc_temp) + " K");
//   Serial.println("Solar Rad = " + String(rad) +" W/m^2");
//   Serial.println();
//   return rad;
  
// }

// double readTerrestrialRadiation() {
//   double tc_temp = 0;
//   double ref_temp = 0;
//   for (int i = 0; i < 10; i++) {
//     tc_temp += terrestrial.readCelsius() + 273.15;
//     ref_temp += readRefTemp();
//   }
//   tc_temp /= 10;
//   ref_temp /= 10;
  
//   double tc = STEFAN_BOLTZMANN*pow(tc_temp,4);
//   double ref = STEFAN_BOLTZMANN*pow(readRefTemp(),4);
//   double rad = abs(ref-tc);
//   Serial.println("Reference Temp = " + String(ref_temp) + " K");
//   Serial.println("Terrestrial Temp = " + String(tc_temp) + " K");
//   Serial.println("Terrestrial Rad = " + String(rad) +" W/m^2");
//   Serial.println();
//   return rad;
// }

// void setup() {
//   Serial.begin(9600);
//   dht.begin();
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
//   delay(2000);
//   readTerrestrialRadiation();
//   readSolarRadiation();
//   //  delay(1000);
//   // Serial.println(bme.readTemperature());
// }