#ifndef A93137F1_3D59_4BCC_AB39_F7BC8A5C538F
#define A93137F1_3D59_4BCC_AB39_F7BC8A5C538F

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>

#include "constants.h"
#include "pindefs.h"
#include "sensors.h"
#include "settings.h"

void updateDisplay(Adafruit_SSD1306 &oled, Adafruit_ILI9341 &tft,
                   const SensorData &data, Settings &settings,
                   const TinyGPSPlus &gps, int frame);

// OLED display frames
// enum Frames { SENSORS = 0, GPSDATA = 1, SETTINGS = 2 };
enum Menus { MAINMENU, SENSORS, SETTINGS, NUM_MENUS };

uint8_t tftState = 0;
uint8_t substate = 0;
uint8_t sensorScreenPos = 0;
bool redraw = true;

Settings settings;
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

bool useOled = true;
volatile bool isRightButtonPressed = false;
volatile bool isLeftButtonPressed = false;
volatile bool isSelectButtonPressed = false;

/**
    Set up the OLED display
*/
void init_oled() {
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("OLED display allocation failed"));
    while (1)
      ;  // Don't proceed, loop forever
  }
  oled.clearDisplay();
  oled.display();
  oled.setTextColor(WHITE);
}

/**
 * @brief Start up the TFT LCD display
 *
 */
void init_tft() {
  SPI.begin(SCK, MISO, MOSI, TFT_CS);
  tft.begin();
  tft.setRotation(3);
  if (tft.readcommand8(ILI9341_RDMODE)) {
    useOled = false;
  } else {
    Serial.println("TFT not connected");
  }
}

/**
   Updates the OLED display

   This function gets called when it's time to update the data, or if a new
   frame or line is requested.

   INPUTS:
      - frame: The display frame index after the update
      - line: The index of the selected line (after updating)
*/
void updateDisplay(Adafruit_SSD1306 &oled, Adafruit_ILI9341 &tft,
                   const SensorData &data, Settings &settings,
                   const TinyGPSPlus &gps, int state) {
  if (useOled) {
    // oled.clearDisplay();
    // oled.setCursor(0, 0);
    // switch (state) {
    //   case SENSORS: {  // Display sensor data
    //     oled.setTextSize(2);
    //     oled.println("Sensors");
    //     oled.setTextSize(1);
    //     oled.print("Temperature: ");
    //     oled.print(data.temperature);
    //     oled.println(settings.temperatureUnit == FAHRENHEIT ? " F" : " C");
    //     oled.println("Humidity: " + String((int)data.humidity) + "%");
    //     oled.println("Pressure: " + String(data.pressure) + " hPa");
    //     oled.println("Wind speed: " + String(data.windSpeed) + " mph");
    //     oled.display();
    //   } break;

    //   case GPSDATA: {  // Display GPS data
    //     oled.setTextSize(2);
    //     oled.println("GPS Data");
    //     oled.setTextSize(1);

    //     oled.println("Time: " + String(data.hour) + ":" + String(data.minute)
    //     +
    //                  ":" + String(data.second) + " UTC");
    //     if (not gps.location.isValid()) {
    //       oled.println("Connecting...");
    //     } else {
    //       oled.println("Latitude: " + String(abs(data.lat)) +
    //                    (data.lat > 0 ? " N" : " S"));
    //       oled.println("Longitude: " + String(abs(data.lon)) +
    //                    (data.lon > 0 ? " E" : " W"));
    //       oled.println("Altitude: " + String(data.alt) + "m");
    //       oled.println("Speed: " + String(data.speed) + " mph");
    //     }

    //   } break;

    // case SETTINGS: {  // Display the settings tab
    //   int vpos = 0;
    //   oled.setTextSize(2);
    //   oled.println("Settings");
    //   vpos += 16;
    //   oled.setTextSize(1);
    //   oled.setCursor(10, vpos);
    //   oled.println(
    //       "Disp. Rate: " +
    //       String(settings.intervals[settings.displayUpdateIntervalIndex]) +
    //       " s");
    //   vpos += 10;
    //   oled.setCursor(10, vpos);
    //   oled.println(
    //       "Data Rate: " +
    //       String(settings.intervals[settings.dataUpdateIntervalIndex]) +
    //       " s");
    //   vpos += 10;
    //   oled.setCursor(10, vpos);
    //   oled.println(
    //       "Temp. Unit: " +
    //       String(settings.temperatureUnit == FAHRENHEIT ? "F" : "C"));
    //   vpos += 10;
    //   oled.setCursor(0, 16 + 10 * line);
    //   oled.print("> ");

    // } break;
    // default:
    //   break;
    // }
    // oled.display();
  } else {
    switch (state) {
      case MAINMENU:
        if (redraw) {
          redraw = false;
          tft.fillScreen(ILI9341_BLUE);
          tft.setTextSize(3);
          tft.setCursor(80, 10);
          tft.setTextColor(ILI9341_WHITE);
          tft.println("MAIN MENU");
          tft.setTextSize(1);
          // Sensor readings box
          tft.fillRect(20, 50, 130, 50, ILI9341_BLACK);
          tft.drawRect(20, 50, 130, 50, ILI9341_WHITE);
          // Settings box
          tft.fillRect(20, 110, 130, 50, ILI9341_BLACK);
          tft.drawRect(20, 110, 130, 50, ILI9341_WHITE);
          // Sleep box
          tft.fillRect(20, 170, 130, 50, ILI9341_BLACK);
          tft.drawRect(20, 170, 130, 50, ILI9341_WHITE);
        }
        substate = substate % 3;
        tft.setTextSize(1);
        tft.setTextColor(ILI9341_WHITE);
        switch (substate) {
          case 0:
            tft.setCursor(40, 130);
            tft.println("Settings");
            tft.setCursor(40, 190);
            tft.println("Sleep");
            tft.setTextColor(ILI9341_GREEN);
            tft.setCursor(40, 70);
            tft.println("Sensor Readings");
            if (isSelectButtonPressed) {
              tftState = SENSORS;
              sensorScreenPos = 0;
              substate = 0;
              redraw = true;
            }
            break;
          case 1:
            tft.setCursor(40, 70);
            tft.println("Sensor Readings");
            tft.setCursor(40, 190);
            tft.println("Sleep");
            tft.setTextColor(ILI9341_GREEN);
            tft.setCursor(40, 130);
            tft.println("Settings");
            if (isSelectButtonPressed) {
              tftState = SETTINGS;
              substate = 0;
              redraw = true;
            }
            break;
          case 2:
            tft.setCursor(40, 70);
            tft.println("Sensor Readings");
            tft.setCursor(40, 130);
            tft.println("Settings");
            tft.setTextColor(ILI9341_GREEN);
            tft.setCursor(40, 190);
            tft.println("Sleep");
            break;
        }
        break;

      case SENSORS:
        tft.setTextColor(ILI9341_WHITE);
        if (redraw) {
          redraw = false;
          tft.fillScreen(ILI9341_BLUE);
          // tft.setTextColor(ILI9341_WHITE);
          tft.setTextSize(3);
          tft.fillRect(90, 6, 145, 30, ILI9341_BLACK);
          tft.drawRect(90, 6, 145, 30, ILI9341_WHITE);
          // Down button
          tft.fillRect(20, 180, 60, 50, ILI9341_BLACK);
          tft.drawRect(20, 180, 60, 50, ILI9341_WHITE);
          // Up button
          tft.fillRect(90, 180, 60, 50, ILI9341_BLACK);
          tft.drawRect(90, 180, 60, 50, ILI9341_WHITE);
          // Back button
          tft.fillRect(180, 180, 120, 50, ILI9341_BLACK);
          tft.drawRect(180, 180, 120, 50, ILI9341_WHITE);
          tft.setCursor(100, 10);
          tft.println("SENSORS");
        }
        tft.setTextSize(2);
        if (sensorScreenPos == 0) {
          tft.fillRect(160, 60, 160, 120, ILI9341_BLUE);
          tft.setCursor(15, 60);
          tft.print("TEMPERATURE: ");
          tft.setCursor(160, 60);
          tft.print(String((int)data.temperature % 100) +
                      (settings.temperatureUnit == FAHRENHEIT ? "F" : "C"));
          tft.setCursor(15, 80);
          tft.print("HUMIDITY: ");
          tft.setCursor(160, 80);
          tft.print(String(100) + " %");
          tft.setCursor(15, 100);
          tft.print("UV INDEX: ");
          // TODO: Average UV sensor readings
          tft.setCursor(160, 100);
          tft.print(1337);
          tft.setCursor(15, 120);
          tft.print("WIND SPEED: ");
          tft.setCursor(160, 120);
          tft.print(String(data.windSpeed) + " mph");
          tft.setCursor(15, 140);
          tft.print("WIND DIR.: ");
          tft.setCursor(160, 140);
          tft.print(String(data.windDirection) + " deg");
        } else {
          tft.setCursor(15, 60);
          tft.print("TIME: ");
          tft.println(String(data.hour) + ":" + String(data.minute) + ":" +
                      String(data.second) + " UTC");
          if (not gps.location.isValid()) {
            tft.println("Connecting...");
          } else {
            tft.setCursor(15, 80);
            tft.println("LATITUDE: " + String(abs(data.lat)) +
                        (data.lat > 0 ? " N" : " S"));
            tft.setCursor(15, 100);
            tft.println("Longitude: " + String(abs(data.lon)) +
                        (data.lon > 0 ? " E" : " W"));
            tft.setCursor(15, 120);
            tft.println("Altitude: " + String(data.alt) + " m");
            tft.setCursor(15, 140);
            tft.println("Speed: " + String(data.speed) + " mph");
          }
        }
        substate = substate % 3;
        switch (substate) {
          case 0:
            // Highlight the buttons
            tft.setCursor(105, 195);
            tft.println("->");
            tft.setCursor(205, 195);
            tft.println("<<<");
            tft.setTextColor(ILI9341_GREEN);
            tft.setCursor(35, 195);
            tft.println("<-");
            // Select button pressed
            if (isSelectButtonPressed && sensorScreenPos == 1) {
              sensorScreenPos = 0;
              redraw = true;
            }
            break;
          case 1:
            tft.setCursor(35, 195);
            tft.println("<-");
            tft.setCursor(205, 195);
            tft.println("<<<");
            tft.setTextColor(ILI9341_GREEN);
            tft.setCursor(105, 195);
            tft.println("->");
            if (isSelectButtonPressed && sensorScreenPos == 0) {
              sensorScreenPos = 1;
              redraw = true;
            }
            break;
          case 2:
            tft.setCursor(35, 195);
            tft.println("<-");
            tft.setCursor(105, 195);
            tft.println("->");
            tft.setTextColor(ILI9341_GREEN);
            tft.setCursor(205, 195);
            tft.println("<<<");
            if (isSelectButtonPressed) {
              tftState = MAINMENU;
              substate = 0;
              redraw = true;
              // This shouldn't be needed, but it is for some reason
              isSelectButtonPressed = false;
            }
            break;
        }
        break;

      case SETTINGS:
        if (redraw) {
          redraw = false;
          tft.fillScreen(ILI9341_BLUE);
          tft.setTextColor(ILI9341_WHITE);
          tft.setTextSize(3);
          tft.setCursor(100, 10);
          tft.println("SETTINGS");
          // Back button
          tft.fillRect(180, 180, 120, 50, ILI9341_BLACK);
          tft.drawRect(180, 180, 120, 50, ILI9341_WHITE);
        }
        substate = substate % 5;
        tft.setTextColor(ILI9341_WHITE);
        tft.setTextSize(2);
        switch (substate) {
          case 0:  // Temperature unit field selected
            tft.setCursor(15, 80);
            tft.print("DISPLAY UPDATE: ");
            tft.print(settings.intervals[settings.displayUpdateIntervalIndex]);
            tft.println(" s");
            tft.setCursor(15, 100);
            tft.print("DATA UPDATE: ");
            tft.print(settings.intervals[settings.dataUpdateIntervalIndex]);
            tft.println(" s");
            tft.setCursor(15, 120);
            tft.print("OLED SCREEN: ");
            tft.println(settings.useOled ? "ON" : "OFF");
            tft.setCursor(205, 195);
            tft.println("<<<");
            tft.setTextColor(ILI9341_GREEN);
            tft.setCursor(15, 60);
            tft.print("TEMPERATURE UNIT: ");
            tft.println(settings.temperatureUnit == FAHRENHEIT ? "F" : "C");
            if (isSelectButtonPressed) {
              settings.temperatureUnit =
                  (settings.temperatureUnit == FAHRENHEIT) ? CELSIUS
                                                           : FAHRENHEIT;
            }
            break;

          case 1:  // Display update interval field selected
            tft.setCursor(15, 60);
            tft.print("TEMPERATURE UNIT: ");
            tft.println(settings.temperatureUnit == FAHRENHEIT ? "F" : "C");
            tft.setCursor(15, 100);
            tft.print("DATA UPDATE: ");
            tft.print(settings.intervals[settings.dataUpdateIntervalIndex]);
            tft.println(" s");
            tft.setCursor(15, 120);
            tft.print("OLED SCREEN: ");
            tft.println(settings.useOled ? "ON" : "OFF");
            tft.setCursor(205, 195);
            tft.println("<<<");
            tft.setTextColor(ILI9341_GREEN);
            tft.setCursor(15, 80);
            tft.print("DISPLAY UPDATE: ");
            tft.print(settings.intervals[settings.displayUpdateIntervalIndex]);
            tft.println(" s");
            if (isSelectButtonPressed) {
              settings.displayUpdateIntervalIndex =
                  (settings.displayUpdateIntervalIndex + 1) %
                  settings.intervals.size();
              // redraw = true;
              isSelectButtonPressed = false;
            }
            break;

          case 2:  // Data update interval field selected
            tft.setCursor(15, 60);
            tft.print("TEMPERATURE UNIT: ");
            tft.println(settings.temperatureUnit == FAHRENHEIT ? "F" : "C");
            tft.setCursor(15, 80);
            tft.print("DISPLAY UPDATE: ");
            tft.print(settings.intervals[settings.displayUpdateIntervalIndex]);
            tft.println(" s");
            tft.setCursor(15, 120);
            tft.print("OLED SCREEN: ");
            tft.println(settings.useOled ? "ON" : "OFF");
            tft.setCursor(205, 195);
            tft.println("<<<");
            tft.setTextColor(ILI9341_GREEN);
            tft.setCursor(15, 100);
            tft.print("DATA UPDATE: ");
            tft.print(settings.intervals[settings.dataUpdateIntervalIndex]);
            tft.println(" s");
            if (isSelectButtonPressed) {
              settings.dataUpdateIntervalIndex =
                  (settings.dataUpdateIntervalIndex + 1) %
                  settings.intervals.size();
              // redraw = true;
              isSelectButtonPressed = false;
            }
            break;

          case 3:  // OLED screen field selected
            tft.setCursor(15, 60);
            tft.print("TEMPERATURE UNIT: ");
            tft.println(settings.temperatureUnit == FAHRENHEIT ? "F" : "C");
            tft.setCursor(15, 80);
            tft.print("DISPLAY UPDATE: ");
            tft.print(settings.intervals[settings.displayUpdateIntervalIndex]);
            tft.println(" s");
            tft.setCursor(15, 100);
            tft.print("DATA UPDATE: ");
            tft.print(settings.intervals[settings.dataUpdateIntervalIndex]);
            tft.println(" s");
            tft.setCursor(205, 195);
            tft.println("<<<");
            tft.setTextColor(ILI9341_GREEN);
            tft.setCursor(15, 120);
            tft.print("OLED SCREEN: ");
            tft.println(settings.useOled ? "ON" : "OFF");
            if (isSelectButtonPressed) {
              settings.useOled = !settings.useOled;
              // redraw = true;
              isSelectButtonPressed = false;
            }
            break;

          case 4:  // Back button selected
            tft.setCursor(15, 60);
            tft.print("TEMPERATURE UNIT: ");
            tft.println(settings.temperatureUnit == FAHRENHEIT ? "F" : "C");
            tft.setCursor(15, 80);
            tft.print("DISPLAY UPDATE: ");
            tft.print(settings.intervals[settings.displayUpdateIntervalIndex]);
            tft.println(" s");
            tft.setCursor(15, 100);
            tft.print("DATA UPDATE: ");
            tft.print(settings.intervals[settings.dataUpdateIntervalIndex]);
            tft.println(" s");
            tft.setCursor(15, 120);
            tft.print("OLED SCREEN: ");
            tft.println(settings.useOled ? "ON" : "OFF");
            tft.setTextColor(ILI9341_GREEN);
            tft.setCursor(205, 195);
            tft.println("<<<");
            if (isSelectButtonPressed) {
              substate = 0;
              redraw = true;
              tftState = MAINMENU;
              isSelectButtonPressed = false;
            }
            break;
        }
    }
  }
}

#endif /* A93137F1_3D59_4BCC_AB39_F7BC8A5C538F */
