#include <fs.h>
#include <SPIFFS.h>
#include <stdlib.h>   
#include <ArduinoJson.h>

#include <AzureIoTHub.h>

#include "device_configuration.h"
#include "logging.h"
#include "constants.h"
#include "blink.h"
#include "iot_device_core.h"
#include "iot_device_twin.h"

#define PROPERTIES_FILE_NAME_LENGTH 128

#define PROPERTIES_DESIRED_NAME "desired"
#define PROPERTIES_REPORTED_NAME "reported"

bool __twin_received__;

void IoTDevice_PropertiesWrite(const char* stateName, DynamicJsonDocument state){
  char configFileName[128]; 

  strcpy(configFileName, CONFIGURATION_FILE_PATH);
  strcat(configFileName, "/state-");
  strcat(configFileName, stateName);
  strcat(configFileName, ".json");

  char* content = (char *) malloc(measureJson(state) + 1 );
  size_t size = serializeJson(state, content, measureJson(state) + 1);

  DeviceConfiguration_WriteFileContent(configFileName, content, size);
}

DynamicJsonDocument IoTDevice_PropertiesRead(const char* stateName){
  char configFileName[128]; 
  size_t size;

  strcpy(configFileName, CONFIGURATION_FILE_PATH);
  strcat(configFileName, "/state-");
  strcat(configFileName, stateName);
  strcat(configFileName, ".json");

  DynamicJsonDocument deviceTwin(TWIN_SIZE);

  File file = SPIFFS.open(configFileName, "r");
  DeserializationError e = deserializeJson(deviceTwin, file);

  if(e.code() != e.Ok){
    Log_Error("Unable to parse twin: %s", e.c_str());
  }

  file.close();

  return deviceTwin; 
}

static void IoTDevice_UpdateCompleteProperties(const unsigned char *payLoad, size_t size){
  Log_Info("Device Twin update called for COMPLETE\n%s", payLoad);

  DynamicJsonDocument deviceTwin(TWIN_SIZE);
  DeserializationError e = deserializeJson(deviceTwin, payLoad, size);

  if(e.code() != e.Ok){
    Log_Error("Unable to parse twin: %s", e.c_str());
  }
  
  IoTDevice_PropertiesWrite(PROPERTIES_DESIRED_NAME, deviceTwin["desired"].as<JsonVariant>());
  IoTDevice_PropertiesWrite(PROPERTIES_REPORTED_NAME, deviceTwin["reported"].as<JsonVariant>());

  __twin_received__ = true;
}

static void IoTDevice_UpdateDesiredProperties(const unsigned char *payLoad, size_t size){
  Log_Info("Device Twin update called for PARTIAL");
  Log_Debug("Patch device twin :\r\n%.*s\r\n", (int)size, payLoad);
  
  DynamicJsonDocument deviceTwin(TWIN_SIZE);

  DeserializationError e = deserializeJson(deviceTwin, payLoad, size);

  if(e.code() != e.Ok){
    Log_Error("Unable to patch desired properties: %s", e.c_str());
  }

  IoTDevice_PropertiesWrite(PROPERTIES_DESIRED_NAME, deviceTwin.as<JsonVariant>());
}

static void IoTDevice_SendReportedPropertiesCallback(int status_code, void *userContextCallback){
  BlinkLed_Blink();
  Log_Info("Device Twin reported properties update completed with result: %d", status_code);

  (void)userContextCallback;
}

void IoTDevice_DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE update_state, const unsigned char *payLoad, size_t size, void *userContextCallback){
  (void)update_state;
  (void)size;
  Log_Trace("IoTDevice_DeviceTwinCallback");

  switch (update_state)
  {
  case DEVICE_TWIN_UPDATE_COMPLETE:
    IoTDevice_UpdateCompleteProperties(payLoad, size);
    break;
  case DEVICE_TWIN_UPDATE_PARTIAL:
    IoTDevice_UpdateDesiredProperties(payLoad, size);
    break;
  }

  (void)userContextCallback;
}

DynamicJsonDocument IoTDevice_GetDesiredProperties()
{      
  return IoTDevice_PropertiesRead(PROPERTIES_DESIRED_NAME);
}

DynamicJsonDocument IoTDevice_GetReportedProperties()
{  
  return IoTDevice_PropertiesRead(PROPERTIES_REPORTED_NAME);
}

void IoTDevice_ReportState(DynamicJsonDocument reportedState)
{
  Log_Info("Sending reported properties state");

  char* content = (char *) malloc(measureJson(reportedState) + 1 );
  size_t size = serializeJson(reportedState, content, measureJson(reportedState) + 1);

  Log_Debug("Reporting twin update %d: %s", size, content);
  IOTHUB_CLIENT_RESULT result = IoTHubClient_LL_SendReportedState(__hub_client_handle__, (const unsigned char *)content, size, IoTDevice_SendReportedPropertiesCallback, NULL);
  
  if(result != IOTHUB_CLIENT_OK)
  {
    char * resultCode = IoTDevice_GetErrorText(result);

    Log_Error("Failed to send reported properties => %s", resultCode);
    free(resultCode);
  }

  IoTDevice_PropertiesWrite(PROPERTIES_REPORTED_NAME, reportedState);
}