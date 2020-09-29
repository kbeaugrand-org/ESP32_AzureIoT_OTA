/**
 * A simple Azure IoT example for sending telemetry.
 */

#include <Arduino.h>
#include <ArduinoJson.h>

#include "inc/DeviceConfiguration.hpp"
#include "inc/IoTDevice.hpp"
#include "inc/Blink.hpp"

IoTDevice iot;
Blink_LED led;

void setup()
{
  pinMode(BLINK_LED_PIN, OUTPUT);
  Serial.begin(115200);

  DeviceConfiguration::init();

  iot.connect();

  randomSeed(analogRead(0));
}

void loop()
{  
  if(!iot.isConnected()){
    led.blink();
    return;
  }

  DynamicJsonDocument doc(1024);

  doc["random"] = random(300);

  doc["sensor"] = "gps";
  doc["data"][0] = 48.756080;
  doc["data"][1] = 2.302038;

  iot.send(doc);

  serializeJson(iot.getProperties(), Serial);
  delay(iot.getProperties()["telemetry"]["frequency"].as<int>());

  delay(2000);
}