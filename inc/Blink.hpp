#include <Arduino.h>

/*
  Blink.h - Library for Blink the integrated LED on Arduino cards.
*/
#ifndef Blink_h
#define Blink_h

#define BLINK_LED_PIN 2

class Blink_LED
{
public:

  void blink()
  {
    this->high();
    delay(50);

    this->low();
    delay(150);
  }

  void high()
  {
    digitalWrite(BLINK_LED_PIN, HIGH);
  }

  void low()
  {
    digitalWrite(BLINK_LED_PIN, LOW);
  }
};

#endif