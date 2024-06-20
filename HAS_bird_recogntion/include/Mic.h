//
// Created by Ewout on 8-6-2023.
//

#pragma once
#include <vector>
#include "Arduino.h"
#include "SDCardReaderAndWriter.h"

#define SAMPLE_RATE 16000
#define SAMPLE_TIME 10

class Mic {
public:
  typedef struct {
	float* data;
	uint32_t size;
  } audio_buffer_t;

  Mic();
  void tick();
  bool begin();
  audio_buffer_t audioBufferGet();
  bool audioBufferReady() const;
  bool audioBufferClear();
  void SaveAudio(SDCardReaderAndWriter* sd);

private:
  float* buffer = nullptr;
  uint32_t* bufferInt = nullptr;
  volatile uint32_t currentSample = 0;

  [[noreturn]] static void thread(void *arg);

};
