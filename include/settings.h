#ifndef A16CA5A1_216C_49F3_A96D_0B658C277D6A
#define A16CA5A1_216C_49F3_A96D_0B658C277D6A
#include <vector>

// Possible temperature units
enum TemperatureUnit { CELSIUS, FAHRENHEIT };

struct Settings {
  std::vector<uint8_t> intervals = {1, 2, 5, 10};
  uint8_t displayUpdateIntervalIndex = 0;
  uint8_t dataUpdateIntervalIndex = 0;
  uint8_t temperatureUnit = FAHRENHEIT;
  bool useOled = true;
};


#endif /* A16CA5A1_216C_49F3_A96D_0B658C277D6A */
