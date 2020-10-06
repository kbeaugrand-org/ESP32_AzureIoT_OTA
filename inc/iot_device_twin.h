#ifndef IOT_DEVICE_TWIN_H
#define IOT_DEVICE_TWIN_H

#include <ArduinoJson.h>

#include "constants.h"

StaticJsonDocument<TWIN_SIZE> IoTDevice_GetDesiredProperties();

#endif