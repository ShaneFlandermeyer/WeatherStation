#ifndef A93137F1_3D59_4BCC_AB39_F7BC8A5C538F
#define A93137F1_3D59_4BCC_AB39_F7BC8A5C538F

#include "constants.h"
#include "sensors.h"
#include "settings.h"
#include "pindefs.h"

void updateDisplay(Adafruit_SSD1306 &display, SensorData data, gps_fix gpsData,
                   int frame, int line);

// OLED display frames
enum Frames { SENSORS = 0, GPSDATA = 1, SETTINGS = 2 };


Settings settings;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

/**
    Set up the display
*/
void init_display() {
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("OLED display allocation failed"));
    while (1)
      ;  // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE);
}

/**
   Updates the OLED display

   This function gets called when it's time to update the data, or if a new
   frame or line is requested.

   INPUTS:
      - frame: The display frame index after the update
      - line: The index of the selected line (after updating)
*/
void updateDisplay(Adafruit_SSD1306 &display, SensorData data,
                   Settings settings, gps_fix gpsData, int frame, int line) {
  display.clearDisplay();
  display.setCursor(0, 0);
  switch (frame) {
    case SENSORS: {  // Display sensor data
      display.setTextSize(2);
      display.println("Sensors");
      display.setTextSize(1);
      display.print("Temperature: ");
      display.print(data.temperature);
      display.println(settings.temperatureUnit == FAHRENHEIT ? " F" : " C");
      display.println("Humidity: " + String((int)data.humidity) + "%");
      display.println("Pressure: " + String(data.pressure) + " hPa");
      display.println("Wind speed: " + String(data.windSpeed) + " mph");
      display.display();
    } break;

    case GPSDATA: {  // Display GPS data
      display.setTextSize(2);
      display.println("GPS Data");
      display.setTextSize(1);
      if (gpsData.status == gps_fix::STATUS_NONE) {
        display.println("Connecting...");
      }
      display.println("Time: " + String(gpsData.dateTime.hours) + ":" +
                      String(gpsData.dateTime.minutes) + ":" +
                      String(gpsData.dateTime.seconds) + " UTC");
      display.println("Latitude: " + String(abs(data.lat)) +
                      (data.lat > 0 ? " N" : " S"));
      display.println("Longitude: " + String(abs(data.lon)) +
                      (data.lon > 0 ? " E" : " W"));
      display.println("Speed: " + String(data.speed) + " mph");
    } break;

    case SETTINGS: {  // Display the settings tab
      int vpos = 0;
      display.setTextSize(2);
      display.println("Settings");
      vpos += 16;
      display.setTextSize(1);
      display.setCursor(10, vpos);
      display.println(
          "Disp. Rate: " +
          String(settings.intervals[settings.displayUpdateIntervalIndex]) +
          " s");
      vpos += 10;
      display.setCursor(10, vpos);
      display.println(
          "Data Rate: " +
          String(settings.intervals[settings.dataUpdateIntervalIndex]) + " s");
      vpos += 10;
      display.setCursor(10, vpos);
      display.println(
          "Temp. Unit: " +
          String(settings.temperatureUnit == FAHRENHEIT ? "F" : "C"));
      vpos += 10;
      display.setCursor(0, 16 + 10 * line);
      display.print("> ");

    } break;
    default:
      break;
  }
  display.display();
}

#endif /* A93137F1_3D59_4BCC_AB39_F7BC8A5C538F */
