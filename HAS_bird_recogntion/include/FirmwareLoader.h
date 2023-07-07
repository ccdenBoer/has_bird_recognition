#pragma once
#include <Arduino.h>


struct tfLiteModel_t {
  uint8_t *data;
  uint32_t size;
};

tfLiteModel_t loadTfliteModel();
