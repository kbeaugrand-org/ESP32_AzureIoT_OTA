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

/**
* @brief   Initializes the Blink Led configuration.
*
*            This is a blocking call.
*
*/
void BlinkLed_Init();

/**
* @brief   Realizes a blink on Led.
*
*            This is a blocking call.
*
*/
void BlinkLed_Blink();

/**
* @brief   Sets the Led High.
*
*            This is a blocking call.
*
*/
void BlinkLed_High();

/**
* @brief   Sets the Led Low.
*
*            This is a blocking call.
*
*/
void BlinkLed_Low();

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif