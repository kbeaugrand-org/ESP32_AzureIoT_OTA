#include <Arduino.h>

#ifndef Constants_h
#define Constants_h

const char *configurationFilePath = "/device";
const char *twinFilePath = "properties.json";
const char *stateFilePath = "state.json";
const size_t defaultTwinSize = 1024;

#endif