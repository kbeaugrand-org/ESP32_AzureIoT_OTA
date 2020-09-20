/**
 * A simple Azure IoT example for sending telemetry.
 */

#include <Arduino.h>
#include <ArduinoJson.h>

#include "inc/DeviceConfiguration.hpp"
#include "inc/IoTDevice.hpp"
#include "inc/Blink.hpp"

IoTDevice iot;

void setup()
{
  pinMode(BLINK_LED_PIN, OUTPUT);
  Serial.begin(115200);

  DeviceConfiguration::init();

  iot.connect();
}

void loop()
{  
  DynamicJsonDocument doc(1024);

  doc["sensor"] = "gps";
  doc["data"][0] = 48.756080;
  doc["data"][1] = 2.302038;

  iot.send(doc);

  delay(1000);
}
