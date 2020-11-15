/**
 * A simple Azure IoT example for sending telemetry.
 */
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP32_AzureIoT_OTA.h>
#include <DHTesp.h>

#define DEFAULT_TELEMETRY_FREQUENCY 5000

#define FIRMWARE_VERSION "0.0.2"

DHTesp dht;

bool rebootRequired;

DynamicJsonDocument CheckPropertiesState()
{
  bool hasChanged = false;

  DynamicJsonDocument propertiesState = IoTDevice_GetReportedProperties(); 
  DynamicJsonDocument desiredState = IoTDevice_GetDesiredProperties();   

  if (propertiesState["firmware"]["version"] != FIRMWARE_VERSION)
  {
    propertiesState["firmware"]["version"] = FIRMWARE_VERSION;
    hasChanged = true;
  }  

  if (propertiesState["telemetry"]["frequency"].as<int>() != desiredState["telemetry"]["frequency"].as<int>())
  {
    propertiesState["telemetry"]["frequency"].set(desiredState["telemetry"]["frequency"].as<int>());
    hasChanged = true;
  }

  if(hasChanged){
    IoTDevice_ReportState(propertiesState);
  }

  if (propertiesState["firmware"]["version"] != desiredState["firmware"]["version"])
  {
    IoTDevice_StartOTA();
  }

  return propertiesState;
}

int OnRebootInvoked(const unsigned char* payload, size_t size, unsigned char** response, size_t* response_size)
{
  const char deviceMethodResponse[] = "{ \"acknowledged\": \"true\" }";
  *response_size = sizeof(deviceMethodResponse)-1;
  *response = (unsigned char *)malloc(*response_size);
  (void)memcpy(*response, deviceMethodResponse, *response_size);

  rebootRequired = true;

  return 200;
}

void setup()
{
  Serial.begin(115200);

  IoTDevice_ConnectFromConfiguration();

  // Wait for device connection
  while(!IoTDevice_IsConnected()) { }

  IoTDevice_RegisterForMethodInvoke("reboot", OnRebootInvoked);

  // début de la mesure
  dht.setup(15, DHTesp::DHT11);
}

void loop()
{ 
  if (rebootRequired) {
    ESP.restart();
  }

  DynamicJsonDocument propertiesState = CheckPropertiesState();
  int waitTime = propertiesState["telemetry"]["frequency"] | DEFAULT_TELEMETRY_FREQUENCY;

  TempAndHumidity values = dht.getTempAndHumidity();

  DynamicJsonDocument doc(1024);

  doc["sensor"] = "dht";
  doc["timestamp"] = time(NULL);
  doc["data"]["status"] = dht.getStatusString();
  doc["data"]["temp"]["unit"]  = "C";
  doc["data"]["temp"]["value"]  = String(values.temperature);
  doc["data"]["humidity"]["unit"]  = "%";
  doc["data"]["humidity"]["value"]  = String(values.humidity);

  String output;

  serializeJson(doc, output);
  IoTDevice_Send(output.c_str());
  
  Log_Info("Waiting for next value: %dms", waitTime);
  delay(waitTime);
}