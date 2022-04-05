#ifndef A16CA5A1_216C_49F3_A96D_0B658C277D6A
#define A16CA5A1_216C_49F3_A96D_0B658C277D6A
#include <vector>

// Possible temperature units
enum TemperatureUnit { CELSIUS, FAHRENHEIT };

// Settings names (used for changing settings)
enum SettingNames {
  DISPLAYRATE = 0,
  DATARATE = 1,
  TEMPERATUREUNIT = 2

};

struct Settings {
  std::vector<uint8_t> intervals = {1, 2, 5, 10};
  uint8_t displayUpdateIntervalIndex = 0;
  uint8_t dataUpdateIntervalIndex = 0;
  uint8_t temperatureUnit = FAHRENHEIT;
};


#endif /* A16CA5A1_216C_49F3_A96D_0B658C277D6A */
