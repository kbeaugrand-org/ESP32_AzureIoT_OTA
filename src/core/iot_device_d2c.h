#ifndef __ESP32_AzureIoT_OTA_DEVICED2C_H__
#define __ESP32_AzureIoT_OTA_DEVICED2C_H__

#include <ArduinoJson.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief   Sends the messages to the IoT Hub Gateway.
*
* @param    message    Pointer to a character string containing the message to send.
*
* @return   True if the message was send to the gateway.
*/
bool IoTDevice_Send(const char* message);

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif