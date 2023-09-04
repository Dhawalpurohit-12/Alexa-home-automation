/* This is a demo of an elegant way to run as many relays you want to
 * please check the notes in configuration section below!
 *
 * If you encounter any issues:
 * - check the readme.md at https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md
 * - ensure all dependent libraries are installed
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#arduinoide
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#dependencies
 * - open serial monitor and check whats happening
 * - check full user documentation at https://sinricpro.github.io/esp8266-esp32-sdk
 * - visit https://github.com/sinricpro/esp8266-esp32-sdk/issues and check for existing issues or open a new one
 */

// Uncomment the following line to enable serial debug output
//#define ENABLE_DEBUG
  // ESP8266 Analog Pin ADC0 = A0

int sensorValue = 0;  // value read from the pot

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 

#include <Arduino.h>
#ifdef ESP8266 
       #include <ESP8266WiFi.h>
#endif ""
#ifdef ESP32   
       #include <WiFi.h>
#endif

#include <SinricPro.h>
#include <SinricProSwitch.h>

#ifdef ESP8266
#define RELAYPIN_1 D1
#define RELAYPIN_2 A0
#define RELAYPIN_3 D3
#define RELAYPIN_4 D4
#define RELAYPIN_5 D5
#define RELAYPIN_6 D6
#define RELAYPIN_7 D7
#define RELAYPIN_8 D8
#endif
#ifdef ESP32
#define RELAYPIN_1 16
#define RELAYPIN_2 17
#define RELAYPIN_3 18
#define RELAYPIN_4 19
#define RELAYPIN_5 21
#define RELAYPIN_6 22
#define RELAYPIN_7 23
#define RELAYPIN_8 25
#endif

/*****************
 * Configuration *
 *****************/

struct RelayInfo {
  String deviceId;
  String name;
  int pin;
};

std::vector<RelayInfo> relays = {
    {"6436f429312d40edc3ee70ca", "Relay 1", RELAYPIN_1},
    {"643a3f9e312d40edc3f0bd76", "Relay 2", RELAYPIN_2},
    {"5fxxxxxxxxxxxxxxxxxxxxxx", "Relay 3", RELAYPIN_3},
    {"5fxxxxxxxxxxxxxxxxxxxxxx", "Relay 4", RELAYPIN_4},
    {"5fxxxxxxxxxxxxxxxxxxxxxx", "Relay 5", RELAYPIN_5},
    {"5fxxxxxxxxxxxxxxxxxxxxxx", "Relay 6", RELAYPIN_6},
    {"5fxxxxxxxxxxxxxxxxxxxxxx", "Relay 7", RELAYPIN_7},
    {"5fxxxxxxxxxxxxxxxxxxxxxx", "Relay 8", RELAYPIN_8}};
/*   ^^^^^^^^^^^^^^^^^^^^^^^^^   ^^^^^^^^^  ^^^
 *              |                     |      |
 *              |                     |      +---> digital PIN or GPIO number (see Note below!)
 *              |                     +----------> Name that will be printed to serial monitor
 *              +--------------------------------> deviceId
 * 
 *  In the vector above, you can add as many relays you want to have
 *  This is only limited to:
 *    - the number of SinricPro devices you have available
 *    - the number of pins / GPIOs your board have
 *
 *  Note: Some GPIO's are set to specific level when the board boots up
 *        This might result in strange behavior if there are relays connected to those pins
 *        Check your board documentation!
 */

#define WIFI_SSID  "Nss"
#define WIFI_PASS  "12345678"
#define APP_KEY    "1d301dcb-2d9a-4cf0-a961-868e630dbee1"    // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
#define APP_SECRET "c517b061-9803-406c-8a63-7a92303a526d-0961f6ee-cfbb-442a-bf28-c35602475118" // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"

#define BAUD_RATE  9600              // Change baudrate to your nee
bool onPowerState(const String &deviceId, bool &state) {
  for (auto &relay : relays) {                                                            // for each relay configuration
    if (deviceId == relay.deviceId) {                                                       // check if deviceId matches
      Serial.printf("Device %s turned %s\r\n", relay.name.c_str(), state ? "on" : "off");     // print relay name and state to serial
      digitalWrite(relay.pin, state);                                                         // set state to digital pin / gpio
      return true;                                                                            // return with success true
    }
  }
  return false; // if no relay configuration was found, return false
}

void setupRelayPins() {
  for (auto &relay : relays) {    // for each relay configuration
    pinMode(relay.pin, OUTPUT);     // set pinMode to OUTPUT
  }
}

void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setupSinricPro() {
  for (auto &relay : relays) {                             // for each relay configuration
    SinricProSwitch &mySwitch = SinricPro[relay.deviceId];   // create a new device with deviceId from relay configuration
    mySwitch.onPowerState(onPowerState);                     // attach onPowerState callback to the new device
  }

  SinricPro.onConnected([]() { Serial.printf("Connected to SinricPro\r\n"); });
  SinricPro.onDisconnected([]() { Serial.printf("Disconnected from SinricPro\r\n"); });

  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  Serial.begin(BAUD_RATE);
  setupRelayPins();
  setupWiFi();
  setupSinricPro();
}

void loop() {
  
  SinricPro.handle();
}
