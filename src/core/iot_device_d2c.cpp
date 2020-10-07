#ifndef IOT_DEVICE_D2C_H
#define IOT_DEVICE_D2C_H

#include <ArduinoJson.h>
#include <azure_c_shared_utility/xlogging.h>

#include "iot_device_core.h"
#include "blink.h"

bool IoTDevice_Send(DynamicJsonDocument message)
{
  String output;

  serializeJson(message, output);
  LogInfo("Sending message: %s", output);

  // Construct the iothub message from a string or a byte array
  IOTHUB_MESSAGE_HANDLE messageHandle  = IoTHubMessage_CreateFromString(output.c_str());
  IoTHubDeviceClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, IoTDevice_SendConfirmationCallback, NULL);

  // The message is copied to the sdk so the we can destroy it
  IoTHubMessage_Destroy(messageHandle);
}

void IoTDevice_SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
  if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
  {
    BlinkLed_Blink();
  }
  else
  {
    LogError("Message was not send: %s", result);
  }
}

#endif