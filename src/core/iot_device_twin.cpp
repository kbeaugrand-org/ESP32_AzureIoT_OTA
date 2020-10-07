#include <ArduinoJson.h>
#include <fs.h>
#include <SPIFFS.h>
#include <azure_c_shared_utility/xlogging.h>

#include <AzureIoTHub.h>

#include "constants.h"
#include "iot_device_core.h"

StaticJsonDocument<TWIN_SIZE> IoTDevice_GetDesiredProperties()
{
  fs::File file = SPIFFS.open((CONFIGURATION_FILE_PATH + String("/") + TWIN_FILE_PATH).c_str(), "r");
  StaticJsonDocument<TWIN_SIZE> doc;
  DeserializationError err = deserializeJson(doc, file);

  if (err.code() != err.Ok)
  {
    LogInfo("Unable to parse saved twin: %s", err.c_str());
  } 
  else if(!doc.containsKey("reported") || !doc["reported"].as<boolean>())
  {
    String output;
    serializeJson(doc["state"], output);
    const char* reportedState = output.c_str();
    file.close();

    IoTHubClient_LL_SendReportedState(iotHubClientHandle, (const unsigned char*)reportedState, output.length(), IoTDevice_SendReportedPropertiesCallback, NULL);

    doc["reported"] = true;
    fs::File writeFile = SPIFFS.open((CONFIGURATION_FILE_PATH + String("/") + TWIN_FILE_PATH).c_str(), "w");
    serializeJson(doc, writeFile);
    writeFile.close();
  }

  return doc["state"];
}

static void IoTDevice_SendReportedPropertiesCallback(int status_code, void* userContextCallback){

}