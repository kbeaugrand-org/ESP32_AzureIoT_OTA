#ifndef __ESP32_AzureIoT_OTA_DEVICETWIN_H__
#define __ESP32_AzureIoT_OTA_DEVICETWIN_H__

#include "constants.h"

/**
* @brief   Gets the device desired properties.
*
*
* @return  A Json document representing the desired properties on device twin.
*/
DynamicJsonDocument IoTDevice_GetDesiredProperties();

/**
* @brief   Gets the device reported properties.
*
*
* @return  A Json document representing the reported properties on device twin.
*/
DynamicJsonDocument IoTDevice_GetReportedProperties();

/**
* @brief   Report the device state to its twin.
* 
* @param reportedState The state to report to its device twin.
*/
void IoTDevice_ReportState(DynamicJsonDocument reportedState);

#endif