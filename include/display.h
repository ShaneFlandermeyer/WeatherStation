#ifndef A93137F1_3D59_4BCC_AB39_F7BC8A5C538F
#define A93137F1_3D59_4BCC_AB39_F7BC8A5C538F

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <PCF8574.h>

#include "constants.h"
#include "pindefs.h"
#include "sensors.h"
#include "settings.h"

void updateDisplay(Adafruit_SSD1306 &oled, Adafruit_ILI9341 &tft,
                   const SensorData &data, Settings &settings,
                   const TinyGPSPlus &gps);

// OLED display frames
enum Frames { SENSORDATA, GPSDATA, NUM_OLED_FRAMES };
enum Menus { MAINMENU, SENSORS, SETTINGS, SLEEP, NUM_MENUS };

uint64_t displayTimer = millis();
uint64_t oledFrameTimer = millis();
volatile uint64_t buttonPressTime = 0;
const int debounceDelay = 200;

uint8_t tftState = 0;
uint8_t oledState = 0;
uint8_t substate = 0;
uint8_t sensorScreenPos = 0;
bool redraw = true;

Settings settings;
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);
PCF8574 pcf8574(0x20);

// bool useOled = true;
volatile bool rightButtonPressed = false;
volatile bool leftButtonPressed = false;
volatile bool selectButtonPressed = false;

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
  if (not settings.useOled) {
    oled.ssd1306_command(SSD1306_DISPLAYOFF);
  }
}

/**
 * @brief Start up the TFT LCD display
 *
 */
void init_tft() {
  tft.begin();
  tft.setRotation(3);
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
                   const TinyGPSPlus &gps) {
  if (settings.useOled) {
    oled.clearDisplay();
    oled.setCursor(0, 0);
    switch (oledState) {
      case SENSORDATA: {  // Display sensor data
        oled.setTextSize(2);
        oled.println("Sensors");
        oled.setTextSize(1);
        oled.print("Temperature: ");
        oled.print(data.temperature);
        oled.println(settings.temperatureUnit == FAHRENHEIT ? " F" : " C");
        oled.println("Humidity: " + String((int)data.humidity) + "%");
        oled.println("Pressure: " + String(data.pressure) + " hPa");
        oled.println("Wind speed: " + String(data.windSpeed) + " mph");
        oled.display();
      } break;

      case GPSDATA: {  // Display GPS data
        oled.setTextSize(2);
        oled.println("GPS Data");
        oled.setTextSize(1);

        oled.println("Time: " + String(data.hour) + ":" + String(data.minute) +
                     ":" + String(data.second) + " UTC");
        if (not gps.location.isValid()) {
          oled.println("Connecting...");
        } else {
          oled.println("Latitude: " + String(abs(data.lat), 4U) +
                       (data.lat > 0 ? " N" : " S"));
          oled.println("Longitude: " + String(abs(data.lon), 4U) +
                       (data.lon > 0 ? " E" : " W"));
          oled.println("Altitude: " + String(data.alt) + "m");
          oled.println("Speed: " + String(data.speed) + " mph");
        }

      } break;
      default:
        break;
    }
    oled.display();
  } 
  switch (tftState) {
    case MAINMENU:
      if (redraw) {
        redraw = false;
        tft.fillScreen(ILI9341_BLUE);
        tft.setTextSize(3);
        tft.setCursor(80, 10);
        tft.setTextColor(ILI9341_WHITE);
        tft.println("MAIN MENU");
        // Sensor readings box
        tft.fillRect(20, 50, 240, 50, ILI9341_BLACK);
        tft.drawRect(20, 50, 240, 50, ILI9341_WHITE);
        // Settings box
        tft.fillRect(20, 110, 240, 50, ILI9341_BLACK);
        tft.drawRect(20, 110, 240, 50, ILI9341_WHITE);
        // Sleep box
        tft.fillRect(20, 170, 240, 50, ILI9341_BLACK);
        tft.drawRect(20, 170, 240, 50, ILI9341_WHITE);
      }
      substate = substate % 3;
      tft.setTextSize(2);
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
          break;
        case 1:
          tft.setCursor(40, 70);
          tft.println("Sensor Readings");
          tft.setCursor(40, 190);
          tft.println("Sleep");
          tft.setTextColor(ILI9341_GREEN);
          tft.setCursor(40, 130);
          tft.println("Settings");
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
      uint8_t vpos;
      if (sensorScreenPos == 0) {
        tft.fillRect(160, 50, 160, 120, ILI9341_BLUE);
        vpos = 50;
        tft.setCursor(15, vpos);
        tft.print("TEMPERATURE: ");
        tft.setCursor(160, vpos);
        tft.print(String(data.temperature) +
                  (settings.temperatureUnit == FAHRENHEIT ? " F" : " C"));
        vpos += 20;
        tft.setCursor(15, vpos);
        tft.print("HUMIDITY: ");
        tft.setCursor(160, vpos);
        tft.print(String(data.humidity) + " %");
        vpos += 20;
        tft.setCursor(15, vpos);
        tft.print("WIND SPEED: ");
        tft.setCursor(160, vpos);
        tft.print(String(data.windSpeed) + " mph");
        vpos += 20;
        tft.setCursor(15, vpos);
        tft.print("WIND DIR.: ");
        tft.setCursor(160, vpos);
        tft.print(String(data.windDirection) + " deg");
        vpos += 20;
        tft.setCursor(15, vpos);
        tft.print("SOLAR RAD.: ");
        tft.setCursor(160, vpos);
        tft.print(String(data.solarRadiation) + " W/m^2");
        vpos += 20;
        tft.setCursor(15, vpos);
        tft.print("TERR. RAD.: ");
        tft.setCursor(160, vpos);
        tft.print(String(data.terrestrialRadiation) + " W/m^2");
      } else if (sensorScreenPos == 1) {
        tft.fillRect(160, 60, 160, 120, ILI9341_BLUE);
        vpos = 60;
        tft.setCursor(15, vpos);
        tft.print("TIME: ");
        tft.setCursor(160, vpos);
        tft.println(String(data.hour) + ":" + String(data.minute) + ":" +
                    String(data.second) + " UTC");
        if (gps.location.isValid()) {
          vpos += 20;
          tft.setCursor(15, vpos);
          tft.print("LATITUDE: ");
          tft.setCursor(160, vpos);
          tft.print(String(abs(data.lat),6U) +
                      (data.lat > 0 ? " N" : " S"));
          vpos += 20;
          tft.setCursor(15, vpos);
          tft.print("LONGITUDE: ");
          tft.setCursor(160, vpos);
          tft.print(String(abs(data.lon),6U) +
                      (data.lon > 0 ? " E" : " W"));
          vpos += 20;
          tft.setCursor(15, vpos);
          tft.print("ALTITUDE: ");
          tft.setCursor(160, vpos);
          tft.print(String(data.alt) + " m");
          vpos += 20;
          tft.setCursor(15, vpos);
          tft.print("SPEED: ");
          tft.setCursor(160, vpos);
          tft.print(String(data.speed) + " mph");
        }
      } else {  // UV sensor screen
        tft.fillRect(100, 60, 220, 120, ILI9341_BLUE);
        vpos = 60;
        tft.setCursor(15, vpos);
        tft.print("UV1: ");
        tft.setCursor(100, vpos);
        tft.print(data.uv[0]);
        vpos += 20;
        tft.setCursor(15, vpos);
        tft.print("UV2: ");
        tft.setCursor(100, vpos);
        tft.print(data.uv[1]);
        vpos += 20;
        tft.setCursor(15, vpos);
        tft.print("UV3: ");
        tft.setCursor(100, vpos);
        tft.print(data.uv[2]);
        vpos += 20;
        tft.setCursor(15, vpos);
        tft.print("UV4: ");
        tft.setCursor(100, vpos);
        tft.print(data.uv[3]);
        vpos += 20;
        tft.setCursor(15, vpos);
        tft.print("UV5: ");
        tft.setCursor(100, vpos);
        tft.print(data.uv[4]);
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
          break;
        case 1:
          tft.setCursor(35, 195);
          tft.println("<-");
          tft.setCursor(205, 195);
          tft.println("<<<");
          tft.setTextColor(ILI9341_GREEN);
          tft.setCursor(105, 195);
          tft.println("->");
          break;
        case 2:
          tft.setCursor(35, 195);
          tft.println("<-");
          tft.setCursor(105, 195);
          tft.println("->");
          tft.setTextColor(ILI9341_GREEN);
          tft.setCursor(205, 195);
          tft.println("<<<");
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
      substate = substate % 6;
      tft.setTextColor(ILI9341_WHITE);
      tft.setTextSize(2);
      tft.setCursor(15, 60);
      tft.print("TEMPERATURE UNIT: ");
      tft.setCursor(240, 60);
      tft.print(settings.temperatureUnit == FAHRENHEIT ? "F" : "C");
      tft.setCursor(15, 80);
      tft.print("DISPLAY UPDATE: ");
      tft.setCursor(240, 80);
      tft.print(settings.intervals[settings.displayUpdateIntervalIndex]);
      tft.print(" s");
      tft.setCursor(15, 100);
      tft.print("DATA UPDATE: ");
      tft.setCursor(240, 100);
      tft.print(settings.intervals[settings.dataUpdateIntervalIndex]);
      tft.print(" s");
      tft.setCursor(15, 120);
      tft.print("OLED SCREEN: ");
      tft.setCursor(240, 120);
      tft.print(settings.useOled ? "ON" : "OFF");
      tft.setCursor(15, 140);
      tft.print("OLED SCROLL: ");
      tft.setCursor(240, 140);
      tft.print(settings.intervals[settings.oledScrollRateIntervalIndex]);
      tft.print(" s");
      tft.setCursor(205, 195);
      tft.print("<<<");
      tft.setTextColor(ILI9341_GREEN);

      switch (substate) {
        case 0:  // Temperature unit field selected
          tft.setCursor(15, 60);
          tft.print("TEMPERATURE UNIT: ");
          tft.setCursor(240, 60);
          tft.println(settings.temperatureUnit == FAHRENHEIT ? "F" : "C");
          break;

        case 1:  // Display update interval field selected
          tft.setCursor(15, 80);
          tft.print("DISPLAY UPDATE: ");
          tft.setCursor(240, 80);
          tft.print(settings.intervals[settings.displayUpdateIntervalIndex]);
          tft.println(" s");
          break;

        case 2:  // Data update interval field selected
          tft.setCursor(15, 100);
          tft.print("DATA UPDATE: ");
          tft.setCursor(240, 100);
          tft.print(settings.intervals[settings.dataUpdateIntervalIndex]);
          tft.println(" s");
          break;

        case 3:  // OLED screen field selected
          tft.setCursor(15, 120);
          tft.print("OLED SCREEN: ");
          tft.setCursor(240, 120);
          tft.println(settings.useOled ? "ON" : "OFF");
          break;

        case 4:
          tft.setCursor(15, 140);
          tft.print("OLED SCROLL: ");
          tft.setCursor(240, 140);
          tft.print(settings.intervals[settings.oledScrollRateIntervalIndex]);
          tft.println(" s");
          break;

        case 5:  // Back button selected
          tft.setCursor(205, 195);
          tft.println("<<<");
          break;
      }
      break;

    case SLEEP:
      digitalWrite(TFT_LED, LOW);
      digitalWrite(TFT_CS, LOW);
      digitalWrite(TFT_DC, LOW);
      SPI.transfer(0x10);
      digitalWrite(TFT_CS, HIGH);
      break;
  }
}

/**
 * Update display parameters based on button inputs and the current state
 *
 */
void updateDisplayParams() {
  switch (tftState) {
    case MAINMENU:
      substate = substate % 3;
      switch (substate) {
        case 0:
          tftState = SENSORS;
          sensorScreenPos = 0;
          substate = 0;
          redraw = true;
          break;
        case 1:
          tftState = SETTINGS;
          substate = 0;
          redraw = true;
          break;
        case 2:
          tftState = SLEEP;
          substate = 0;
          redraw = true;
      }
      break;

    case SENSORS:
      substate = substate % 3;
      switch (substate) {
        case 0:
          sensorScreenPos = max(0, sensorScreenPos-1);
          redraw = true;
          break;
        case 1:
          sensorScreenPos = (sensorScreenPos + 1) % 3;
          redraw = true;
          break;
        case 2:
          tftState = MAINMENU;
          substate = 0;
          redraw = true;
          // This shouldn't be needed, but it is for some reason
          break;
      }
      break;

    case SETTINGS:
      switch (substate) {
        case 0:  // Temperature unit field selected
          settings.temperatureUnit =
              (settings.temperatureUnit == FAHRENHEIT) ? CELSIUS : FAHRENHEIT;
          tft.fillRect(160, 60, 160, 20, ILI9341_BLUE);
          break;

        case 1:  // Display update interval field selected
          settings.displayUpdateIntervalIndex =
              (settings.displayUpdateIntervalIndex + 1) %
              settings.intervals.size();
          tft.fillRect(160, 80, 160, 20, ILI9341_BLUE);
          break;

        case 2:  // Data update interval field selected
          settings.dataUpdateIntervalIndex =
              (settings.dataUpdateIntervalIndex + 1) %
              settings.intervals.size();
          tft.fillRect(160, 100, 160, 20, ILI9341_BLUE);
          break;

        case 3:  // OLED screen field selected
          settings.useOled = !settings.useOled;
          if (settings.useOled) {
            oled.ssd1306_command(SSD1306_DISPLAYON);
          } else {
            oled.ssd1306_command(SSD1306_DISPLAYOFF);
          }
          tft.fillRect(160, 120, 160, 20, ILI9341_BLUE);
          break;

        case 4:  // OLED scroll rate field selected
          settings.oledScrollRateIntervalIndex =
              (settings.oledScrollRateIntervalIndex + 1) %
              settings.intervals.size();
          tft.fillRect(160, 140, 160, 20, ILI9341_BLUE);
          break;

        case 5:  // Back button selected
          substate = 0;
          redraw = true;
          tftState = MAINMENU;
          break;
      }
      break;
    case SLEEP:
      digitalWrite(TFT_LED, HIGH);
      digitalWrite(TFT_CS, LOW);
      digitalWrite(TFT_DC, LOW);
      SPI.transfer(0x11);
      digitalWrite(TFT_CS, HIGH);
      tftState = MAINMENU;
      substate = 2;
      redraw = true;
      break;
  }
}

//****************************************************************************
// * Input handling and interrupts
//****************************************************************************

void handleButtonPress() {
  // Check if the user switched to a new menu tab
  if (pcf8574.read(0)) {
    rightButtonPressed = false;
    substate += 1;
    Serial.println(substate);
    updateDisplay(oled, tft, data, settings, gps);
    Serial.println("Right button pressed");
    while (pcf8574.read(0)) {
      delay(50);
    }
  }

  // Check if the user clicked on an item
  if (pcf8574.read(1)) {
    updateDisplayParams();
    selectButtonPressed = false;
    updateDisplay(oled, tft, data, settings, gps);
    Serial.println("Select button pressed");
    while (pcf8574.read(1)) {
      delay(50);
    }
  }

  if (rightButtonPressed) {
    rightButtonPressed = false;
    substate += 1;
    Serial.println(substate);
    updateDisplay(oled, tft, data, settings, gps);
    Serial.println("Right button pressed");
  }

  // Check if the user clicked on an item
  if (selectButtonPressed) {
    updateDisplayParams();
    selectButtonPressed = false;
    updateDisplay(oled, tft, data, settings, gps);
    Serial.println("Select button pressed");
  }
}

void rightButtonPress() {
  int timeNow = millis();
  if (timeNow > buttonPressTime + debounceDelay) {
    rightButtonPressed = true;
    buttonPressTime = timeNow;
  }
}

void selectButtonPress() {
  int timeNow = millis();
  if (timeNow > buttonPressTime + debounceDelay) {
    selectButtonPressed = true;
    buttonPressTime = timeNow;
  }
}

#endif /* A93137F1_3D59_4BCC_AB39_F7BC8A5C538F */
