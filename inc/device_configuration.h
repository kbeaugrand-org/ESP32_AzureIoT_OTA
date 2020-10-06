#include <Arduino.h>

/*
  DeviceConfiguration.h - Library for manipulating Device Configuration.
*/
#ifndef DeviceConfiguration_h
#define DeviceConfiguration_h

void DeviceConfiguration_Init();

const char* DeviceConfiguration_Get(const char *section, const char *key, const size_t max_size);

#endif