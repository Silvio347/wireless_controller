#ifndef MY_UDP_H
#define MY_UDP_H

#include <WiFiUdp.h>
#include <Arduino.h>
#include <WiFi.h>

class MyUDP
{
public:
    MyUDP();                                                              // Construtor
    void begin(uint16_t port);                                            // Inicia o UDP local
    bool send(const String &msg);                                         // Envia um caractere
    String discoverHostIp(uint16_t port = 4211, uint32_t timeout = 3000); // Descobre IP do PC
    void setHost(String ip);                                              // Define o host manualmente
    String getHost();                                                     // Retorna o IP atual

private:
    WiFiUDP udp;         // udp instance
    String hostIp;       // ip of pc
    uint16_t targetPort; // port of pc
    int failedSends = 0; // number of failed sends
};

extern MyUDP myUdp; // global instance of MyUDP

#endif
