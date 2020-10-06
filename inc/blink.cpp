#include <Arduino.h>

/*
  Blink.h - Library for Blink the integrated LED on Arduino cards.
*/
#define BLINK_LED_PIN 2

void BlinkLed_Blink()
{
  BlinkLed_High();
  delay(20);

  BlinkLed_Low();
  delay(50);
}

void BlinkLed_High()
{
  digitalWrite(BLINK_LED_PIN, HIGH);
}

void BlinkLed_Low()
{
  digitalWrite(BLINK_LED_PIN, LOW);
}
