#include <ArduinoJson.h>
#include <fs.h>
#include <SPIFFS.h>
#include "logging.h"

#include <AzureIoTHub.h>

#include "constants.h"
#include "iot_device_core.h"
#include "iot_device_twin.h"

void IoTDevice_SendReportedPropertiesCallback(int status_code, void* userContextCallback){

}

StaticJsonDocument<TWIN_SIZE> IoTDevice_GetDesiredProperties()
{
  fs::File file = SPIFFS.open((CONFIGURATION_FILE_PATH + String("/") + TWIN_FILE_PATH).c_str(), "r");
  StaticJsonDocument<TWIN_SIZE> doc;
  DeserializationError err = deserializeJson(doc, file);

  if (err.code() != err.Ok)
  {
    Log_Info("Unable to parse saved twin: %s", err.c_str());
  } 
  else if(!doc.containsKey("reported") || !doc["reported"].as<boolean>())
  {
    String output;
    serializeJson(doc["state"], output);
    const char* reportedState = output.c_str();
    file.close();

    IoTHubClient_LL_SendReportedState(__hub_client_handle__, (const unsigned char*)reportedState, output.length(), IoTDevice_SendReportedPropertiesCallback, NULL);

    doc["reported"] = true;
    fs::File writeFile = SPIFFS.open((CONFIGURATION_FILE_PATH + String("/") + TWIN_FILE_PATH).c_str(), "w");
    serializeJson(doc, writeFile);
    writeFile.close();
  }

  return doc["state"];
}