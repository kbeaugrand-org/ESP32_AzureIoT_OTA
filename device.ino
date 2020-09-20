/**
 * A simple Azure IoT example for sending telemetry.
 */

#include <Arduino.h>

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
  char buff[128];

  // replace the following line with your data sent to Azure IoTHub
  snprintf(buff, 128, "{\"topic\":\"iot\"}");
  iot.send(buff);

  delay(100);
}
