/*
  Device_OTA.h - Library for flashing device OTA (Over The Air).
*/
#ifndef Device_OTA_h
#define Device_OTA_h

#include <Arduino.h>

void OTA_Start(String host, int port, String bin);

#endif