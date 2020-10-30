#ifndef __ESP32_AzureIoT_OTA_DEVICECORE_H__
#define __ESP32_AzureIoT_OTA_DEVICECORE_H__

#include <AzureIoTHub.h>
#include <ArduinoJson.h>

#include "constants.h"

#ifdef __cplusplus
extern "C"
{
#endif

char *IoTDevice_GetErrorText(IOTHUB_CLIENT_RESULT result);

extern IOTHUB_CLIENT_LL_HANDLE __hub_client_handle__;

extern void IoTDevice_DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char *payLoad, size_t size, void *userContextCallback);

extern bool __twin_received__;

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif