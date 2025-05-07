#include "my_web_server.h"
#include "pages.h"

MyWebServer webServer; // global instance of MyWebServer

MyWebServer::MyWebServer() : server(80) {} // Constructor

// Delay function
void MyWebServer::myDelayMillis(uint16_t tempo)
{
    uint32_t startTime = millis();
    while ((millis() - startTime) < tempo)
        ;
}

// Create Wi-Fi Access Point
void MyWebServer::createAP()
{
    if (apCreated)
        return;

    apCreated = true;

    WiFi.softAP(SSID_ESP, PASSWORD_ESP);
    IPAddress IP = WiFi.softAPIP();

    Serial.println("\nSem Wi-Fi! Criado AP!");
    Serial.print("IP do ESP32: ");
    Serial.println(IP.toString());
}

// Connect to Wi-Fi
void MyWebServer::connectWiFi()
{
    WiFi.mode(WIFI_MODE_APSTA);
    Serial.println("SSID: " + ssid + " ; password: " + password);
    WiFi.begin(ssid, password);

    uint8_t contador_wifi = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        contador_wifi++;
        if (contador_wifi >= TIMEOUT_WIFI)
        {
            createAP();
            myDelayMillis(50);
            contador_wifi = 0;
            break;
        }
        myDelayMillis(500);
    }

    if (WiFi.isConnected())
    {
        apCreated = false;
        Serial.println("\n✅ Conectado ao Wi-Fi");
        Serial.print("IP do ESP32: ");
        Serial.println(WiFi.localIP());
        xTaskCreate(controllerSerialTask, "Controller Serial", 4096, NULL, 3, NULL);
        xTaskCreate(controllerButtonsTask, "Controller Buttons", 4096, NULL, 3, NULL);
        xTaskCreate(controllerAnalogTask, "Analog Task", 4096, NULL, 1, NULL);
    }
    setupRoutes();
}

// Setup routes for web server
void MyWebServer::setupRoutes()
{
    server.on("/", HTTP_GET, std::bind(&MyWebServer::handleRoot, this));
    server.on("/wifi", HTTP_GET, std::bind(&MyWebServer::handleWiFiForm, this));
    server.on("/wifi", HTTP_POST, std::bind(&MyWebServer::handleWiFiForm, this));
    server.on("/firmware", HTTP_GET, std::bind(&MyWebServer::handleFirmwareUpload, this));
    server.on("/upload", HTTP_POST, []() {}, std::bind(&MyWebServer::handleUploadFirmware, this));

    server.begin();
}

// Handle client requests
void MyWebServer::handleClient()
{
    server.handleClient();
}

// Close the server
void MyWebServer::close()
{
    server.close();
}

// Handle root
void MyWebServer::handleRoot()
{
    server.send(200, "text/html", principal);
}

// Handle Wi-Fi form
void MyWebServer::handleWiFiForm()
{
    if (server.method() == HTTP_POST)
    {
        String newSSID = server.arg("ssid");
        String newPassword = server.arg("password");
        Serial.println("new SSID: " + newSSID + " ; new password: " + newPassword);

        if (newSSID.length() > 0 && newPassword.length() > 0)
        {
            eepromManager.saveStringEEPROM(SSID_ADDRESS, newSSID, SSID_MAX_LENGTH);
            eepromManager.saveStringEEPROM(PASSWORD_ADDRESS, newPassword, PASSWORD_MAX_LENGTH);

            server.send(200, "text/html", "<html><body><h1>Wi-Fi updated. Rebooting...</h1></body></html>");

            delay(1000);
            ESP.restart();
            return;
        }
    }
    server.send(200, "text/html", wifi);
}

// Handle firmware upload form
void MyWebServer::handleUploadFirmware()
{
    HTTPUpload &upload = server.upload();

    if (upload.status == UPLOAD_FILE_START)
    {
        String filename = upload.filename;
        if (!filename.endsWith(".bin"))
        {
            server.send(400, "text/html", "<h1>Invalid file type. Only .bin files are allowed.</h1>");
            return;
        }

        if (!Update.begin(UPDATE_SIZE_UNKNOWN))
        {
            server.send(500, "text/html", "<h1>Failed to begin update!</h1>");
            return;
        }
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
        {
            server.send(500, "text/html", "<h1>Failed to write update!</h1>");
            return;
        }
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (Update.end(true))
        {
            server.send(200, "text/html", "<h1>Firmware uploaded and updated successfully!</h1>");
            ESP.restart();
        }
        else
        {
            server.send(500, "text/html", "<h1>Failed to finalize update!</h1>");
        }
    }
}

// Handle firmware upload
void MyWebServer::handleFirmwareUpload()
{
    server.send(200, "text/html", uploadFW);
}