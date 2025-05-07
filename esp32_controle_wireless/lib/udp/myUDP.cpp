#include <myUDP.h>

MyUDP myUdp;

MyUDP::MyUDP()
{
    hostIp = "";
}

// start the udp
void MyUDP::begin(uint16_t port)
{
    targetPort = port;
    udp.begin(port); // pode ser omitido se só usar para envio
}

// define the ip of pc
void MyUDP::setHost(String ip)
{
    hostIp = ip;
}

// get pc ip
String MyUDP::getHost()
{
    return hostIp;
}

// send command to game
bool MyUDP::send(const String& msg)
{
    if (!WiFi.isConnected() || hostIp == "")
        return false;

    udp.beginPacket(hostIp.c_str(), targetPort);
    udp.print(msg);
    if (udp.endPacket() == 1)
    {
        failedSends = 0;
        return true;
    }

    failedSends++;

    if (failedSends >= 5)
    {
        Serial.println("[UDP] ⚠️ Falha consecutiva. Tentando redescobrir IP...");
        failedSends = 0;
        String ip = discoverHostIp();
        if (ip != "")
            Serial.println("[UDP] ✅ Novo IP descoberto: " + ip);
        }

    return false;
}

// Discover the ip of pc
String MyUDP::discoverHostIp(uint16_t port, uint32_t timeout)
{
    WiFiUDP discovery;
    if (!discovery.begin(port))
        return "";

    Serial.println("[DISCOVERY] Iniciando busca por IP do PC...");
    unsigned long start = millis(), lastSend = 0;

    while (millis() - start < timeout)
    {
        if (millis() - lastSend > 1000)
        {
            discovery.beginPacket(IPAddress(255, 255, 255, 255), port);
            discovery.print("who-is-pc");
            discovery.endPacket();
            lastSend = millis();
        }

        char incoming[32] = {0};
        if (discovery.parsePacket() > 0)
        {
            discovery.read(incoming, sizeof(incoming));
            IPAddress ip = discovery.remoteIP();

            Serial.printf("[DISCOVERY] Pacote: %s de %s\n", incoming, ip.toString().c_str());

            if (strcmp(incoming, "esp32-discovery") == 0 && ip[0] == 192 && ip[1] == 168)
            {
                hostIp = ip.toString();
                Serial.printf("[DISCOVERY] ✅ IP do PC encontrado: %s\n", hostIp.c_str());
                return hostIp;
            }
        }
        delay(10);
    }

    Serial.println("[DISCOVERY] ❌ Timeout sem resposta.");
    return "";
}