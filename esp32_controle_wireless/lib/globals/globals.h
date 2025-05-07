#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

#define WDT_TIMER 20
#define SSID_ESP "ESP32"
#define PASSWORD_ESP "1234ssss"
#define TIMEOUT_WIFI 8
#define BUTTON_FIRE 23
// #define BUTTON_PAUSE 14
#define ANALOG_X 32
#define ANALOG_Y 33

// Addresses and their sizes to save in EEPROM
#define LENGHT_EEPROM 1024
#define SSID_ADDRESS 0
#define SSID_MAX_LENGTH 32
#define PASSWORD_ADDRESS (SSID_ADDRESS + SSID_MAX_LENGTH)
#define PASSWORD_MAX_LENGTH 32

void controllerButtonsTask(void *pvParameters);
void controllerSerialTask(void *pvParameters);
void controllerAnalogTask(void *pvParameters);
void wifiandwdtTask(void *pvParameters);
void myDelayMillis(uint16_t tempo);
void handleWebTask(void *pvParameters);
void udpDiscoveryTask(void);

// Wi-Fi settings
extern String ssid;
extern String password;

extern unsigned long serverStartTime; // Start time for the server
extern bool serverRunning;            // Flag to indicate if the server is running
extern uint8_t reconnectWiFi;         // Counter to reconnect WiFi
extern const int udpPort;             // UDP Port to communicate
extern bool apCreated;                // Flag to indicate if AP has created

#endif