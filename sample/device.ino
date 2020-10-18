/**
 * A simple Azure IoT example for sending telemetry.
 */
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <SPIFFSIniFile.h>

#define IOTDEVICE_PROTOCOL_HTTP

#include <ESP32_AzureIoT_OTA.h>

void setup()
{
  Serial.begin(115200);

  IoTDevice_ConnectFromConfiguration();
  randomSeed(analogRead(0));
}

void loop()
{ 
  if(!IoTDevice_IsConnected()) {
    delay(2000);
  }
  
  DynamicJsonDocument doc(1024);

  doc["random"] = random(300);

  doc["sensor"] = "gps";
  doc["data"][0] = 48.756080;
  doc["data"][1] = 2.302038;

  String output;

  serializeJson(doc, output);
  IoTDevice_Send(output.c_str());

  delay(2000);
}