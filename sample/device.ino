/**
 * A simple Azure IoT example for sending telemetry.
 */
#include <Arduino.h>
#include <ArduinoJson.h>

#define IOTDEVICE_PROTOCOL_MQTT

#include <ESP32AzureIoTOTA.h>

void setup()
{
  pinMode(BLINK_LED_PIN, OUTPUT);
  Serial.begin(115200);

  IoTDevice_ConnectFromConfiguration();
  randomSeed(analogRead(0));
}

void loop()
{  
  if(!IoTDevice_IsConnected()){
    BlinkLed_Blink();
    return;
  }

  DynamicJsonDocument doc(1024);

  doc["random"] = random(300);

  doc["sensor"] = "gps";
  doc["data"][0] = 48.756080;
  doc["data"][1] = 2.302038;
  
  IoTDevice_Send(doc);

  StaticJsonDocument<TWIN_SIZE> desiredProperties = IoTDevice_GetDesiredProperties();

  delay(desiredProperties["telemetry"]["frequency"].as<int>());

  delay(2000);
}