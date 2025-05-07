#include "globals.h"

#define DHTPIN 4
#define DHTTYPE DHT22

// Wi-Fi settings
String ssid = "BlueEngineering";
String password = "santarita2023//";

unsigned long serverStartTime = 0;    // Start time for the server
bool serverRunning = true;            // Flag to indicate if the server is running
uint8_t reconnectWiFi = 0;            // Counter to reconnect WiFi
uint8_t thresholdCounter = 0;         // Counter to send alert
bool lastHeatState = false;           // Flag to indicate the last state of the cooler
const int udpPort = 4210;             // UDP Port to communicate
bool apCreated = false;               // Flag to indicate if AP has created