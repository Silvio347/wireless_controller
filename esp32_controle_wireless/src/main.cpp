#include <WiFi.h>
#include <my_web_server.h>
#include <myUDP.h>
#include "esp_task_wdt.h"
#include "esp_wifi.h"

void setup()
{
  Serial.begin(115200);

  eepromManager.setupEEPROM();
  eepromManager.readParamsEEPROM();

  webServer.connectWiFi();
  myUdp.begin(udpPort);

  esp_wifi_set_ps(WIFI_PS_NONE); // Disable Wi-Fi power saving mode

  pinMode(BUTTON_FIRE, INPUT_PULLUP);
  // pinMode(BUTTON_PAUSE, INPUT_PULLUP);
  pinMode(ANALOG_X, INPUT);
  pinMode(ANALOG_Y, INPUT);

  xTaskCreate(wifiandwdtTask, "WDT/WiFi", 4096, NULL, 1, NULL);
  xTaskCreate(handleWebTask, "OTA Task", 4096, NULL, 1, NULL);
}

void loop()
{
  delay(1);
}

// OTA and WiFi form task
void handleWebTask(void *pvParameters)
{
  while (true)
  {
    webServer.handleClient();
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void controllerAnalogTask(void *pvParameters)
{
  const int range = 100;   // -100 ~ +100
  const int deadzone = 30; // zona morta
  int lastX = 0;
  int lastY = 0;

  while (true)
  {
    if (!WiFi.isConnected())
    {
      vTaskDelay(pdMS_TO_TICKS(500));
      continue;
    }

    int rawX = analogRead(ANALOG_X);
    int rawY = analogRead(ANALOG_Y);

    int x = map(rawX, 0, 4095, -range, range);
    int y = map(rawY, 0, 4095, -range, range);

    if (abs(x) < deadzone) x = 0;
    if (abs(y) < deadzone) y = 0;

    if (x != lastX || y != lastY)
    {
      float dx = (float)x;
      float dy = (float)y;

      // Cria a mensagem no formato que o Python espera
      String msg = "x:left,dx:" + String(dx, 2) + ",dy:" + String(dy, 2);
      myUdp.send(msg);

      lastX = x;
      lastY = y;
    }

    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

// Serial controller for send to game (w, a, s, d, l)
void controllerSerialTask(void *pvParameters)
{
  while (true)
  {
    if (!WiFi.isConnected())
      return;

    if (Serial.available())
    {
      myUdp.send(String((char)Serial.read()));
    }
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

// Physical buttons and analogic for send to game
void controllerButtonsTask(void *pvParameters)
{
  bool lastFireState = true;
  bool lastPauseState = true;
  const int debounceDelay = 50; // ms

  while (true)
  {
    if (!WiFi.isConnected())
      return;

    bool currentFireState = digitalRead(BUTTON_FIRE);
    // bool currentPauseState = digitalRead(BUTTON_PAUSE);

    // FIRE
    if (lastFireState == true && currentFireState == false)
    {
      myUdp.send("f");
      vTaskDelay(pdMS_TO_TICKS(debounceDelay));
    }

    // PAUSE
    // if (lastPauseState == true && currentPauseState == false)
    // {
    //   myUdp.send("p");
    //   vTaskDelay(pdMS_TO_TICKS(debounceDelay));
    // }

    lastFireState = currentFireState;
    // lastPauseState = currentPauseState;

    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

// Task for Wi-Fi reconnection and WDT reset
void wifiandwdtTask(void *pvParameters)
{
  esp_task_wdt_init(WDT_TIMER, true);
  esp_task_wdt_add(NULL);

  while (true)
  {
    esp_task_wdt_reset();
    udpDiscoveryTask();

    reconnectWiFi++;
    if (reconnectWiFi >= 30)
    {
      if (WiFi.status() != WL_CONNECTED)
      {
        webServer.connectWiFi();
      }
      reconnectWiFi = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void udpDiscoveryTask()
{
  if (WiFi.isConnected() && myUdp.getHost() == "")
  {
    String ip = myUdp.discoverHostIp(4211);
    if (ip != "")
      Serial.println("[DISCOVERY TASK] ✅ IP descoberto: " + ip);
    else
      Serial.println("[DISCOVERY TASK] ❌ Ainda não encontrou IP do PC...");
  }
}