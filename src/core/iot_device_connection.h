#ifndef __ESP32_AzureIoT_OTA_DEVICECONNECTION_H__
#define __ESP32_AzureIoT_OTA_DEVICECONNECTION_H__

#ifdef __cplusplus
extern "C"
{
#endif

bool IoTDevice_IsConnected();

bool IoTDevice_ConnectFromConfiguration();

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif