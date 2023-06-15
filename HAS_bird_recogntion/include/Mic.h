//
// Created by Ewout on 8-6-2023.
//

#pragma once
#include <vector>
#include "arduino.h"
#include "SdramAllocator.h"

class Mic {
public:
  typedef struct {
	std::vector<float, SDRAMAllocator<float> > data;
	uint32_t size;
  } audio_buffer_t;

  Mic();
  void tick();
  bool begin();
  audio_buffer_t audioBufferGet();
  bool audioBufferReady();
  bool audioBufferClear();

private:
  std::vector<float, SDRAMAllocator<float> > buffer;
  uint32_t currentSample = 0;

  static void thread(void *arg);

};
