#ifndef __ESP32_AzureIoT_OTA_DEVICECONNECTION_H__
#define __ESP32_AzureIoT_OTA_DEVICECONNECTION_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief   Indicates whether the device is connected to the
*           IoT Hub Gateway or not.
*
*            This is a blocking call.
*
* @return    True if the IoT SDK is connected to the IoT Hub Gateway, 
*            False otherwise.
*/
bool IoTDevice_IsConnected();


/**
* @brief    Connects the device to the IoT Hub Gateway
*           The SDK will use configuration file to setup: 
*               - The Wifi Connection
*               - The Device NTP synchronization
*               - The IoT Hub connection.
*
*            This is a blocking call.
*
*/
void IoTDevice_ConnectFromConfiguration();

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif