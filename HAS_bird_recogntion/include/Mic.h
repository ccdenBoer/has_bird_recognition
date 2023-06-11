//
// Created by Ewout on 8-6-2023.
//

#pragma once
#include "arduino.h"
class Mic {
 public:
  typedef struct {
	float *data;
	uint32_t size;
  }  audio_buffer_t;

  Mic();
  void tick();
  bool begin();
  audio_buffer_t audioBufferGet();
  bool audioBufferReady();

 private:
  float* buffer;
  uint32_t currentSample = 0;
  static int16_t i2s24_to_pcm16_h7(uint32_t x);
};
