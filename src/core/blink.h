/*
  Blink.h - Library for Blink the integrated LED on cards.
*/
#ifndef __ESP32_AzureIoT_OTA_BLINK_H__
#define __ESP32_AzureIoT_OTA_BLINK_H__

#define BLINK_LED_PIN 2

#ifdef __cplusplus
extern "C"
{
#endif

void BlinkLed_Init();

void BlinkLed_Blink();

void BlinkLed_High();

void BlinkLed_Low();

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif