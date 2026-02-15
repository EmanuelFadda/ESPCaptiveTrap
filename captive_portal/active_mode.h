#ifndef ACTIVE_MODE_H
#define ACTIVE_MODE_H

#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"

void setupServerPages();
void setupESPLocalNetwork(char* ssid,char* psw,IPAddress accesspoint,IPAddress gateway,IPAddress subnet);

#endif
