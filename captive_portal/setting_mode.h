#ifndef SETTING_MODE_H
#define SETTING_MODE_H

#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"

void setupBLE(String BLEName);
bool a_send(String value);
String s_recv();
void commandHandler();
void authBLEConnection();

#endif
