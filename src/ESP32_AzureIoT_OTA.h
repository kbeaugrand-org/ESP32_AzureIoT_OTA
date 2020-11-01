/*
  IoTDevice.h - Library manipulating IoT Device for Azure IoT Hub.
*/
#ifndef __ESP32_AzureIoT_OTA_H__
#define __ESP32_AzureIoT_OTA_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "core/constants.h"
#include "core/blink.h"
#include "core/device_configuration.h"
#include "core/logging.h"

#include "core/iot_device_connection.h"
#include "core/iot_device_d2c.h"
#include "core/iot_device_ota.h"

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

// Since device twin uses templates that are not compatible with extern "C"
#include "core/iot_device_twin.h"

#endif