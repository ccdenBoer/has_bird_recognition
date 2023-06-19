//
// Created by Ewout on 8-6-2023.
//
#include "Mic.h"
#include "SDRAM.h"
#include "i2s.h"
#include "Scheduler.h"
#include <cfloat>

#define BUFFER_SIZE ((uint32_t)(SAMPLE_RATE * SAMPLE_TIME))

Mic::Mic() {
  __HAL_RCC_HSEM_CLK_ENABLE();
  // Update the SystemCoreClock variable.
  SystemCoreClockUpdate();
  HAL_Init();
  MX_I2S2_Init();
}

bool Mic::begin() {
  buffer = std::vector<float, SdramAllocator<float>>(BUFFER_SIZE);

  auto state = HAL_I2S_GetState(&hi2s2);
  Serial.println("HAL_I2S_GetState returned\t" + String(state));
  if (state != HAL_I2S_STATE_READY)
	return false;

  //start arduino thread
  Scheduler.start(thread, this);

  return true;
}

Mic::audio_buffer_t Mic::audioBufferGet() {
  return {buffer, BUFFER_SIZE};
}


bool Mic::audioBufferReady() {
  return currentSample >= BUFFER_SIZE;
}

#define I2S_BUFFER_SIZE UINT16_MAX
uint32_t i2sBuffer[I2S_BUFFER_SIZE]{0};

void Mic::tick() {
  if (currentSample >= BUFFER_SIZE) {
	delay(100);
	return;
  }

  auto result = HAL_I2S_Receive(&hi2s2, reinterpret_cast<uint16_t *>(i2sBuffer), I2S_BUFFER_SIZE, 1000);
  if (result != HAL_OK) {
	Serial.println("HAL_I2S_Receive failed");
  }

  for (int i = 0; i < I2S_BUFFER_SIZE; i++) {
	auto value = i2sBuffer[i];
	if (value == 0) continue;
	//convert 24bit with the last 6 bits being 0 to 32bit signed
	auto converted = static_cast<int32_t>(i2sBuffer[i] << 8);
	converted = converted >> 14;
	float max = 1 << 17;
	auto f = static_cast<float>(converted);


	//convert 32bit signed to float
	buffer[currentSample++] = (f / max);
	if (currentSample >= BUFFER_SIZE) {
	  break;
	}



  }

//	pcm16 = (int16_t)(i2sBuffer[i] >> 8);
//	if (pcm16 == 0)
//	  continue;
//	if (currentSample >= BUFF_SIZE) {
//	  break;
//	}
//	auto pcmFloat = ((float)pcm16 / (1 << 15));
//	buffer[currentSample++] = pcmFloat;

  printf("currentSample: %lu\n", currentSample);
}

void Mic::thread(void *arg) {
  auto mic = (Mic *)arg;
  while (true) {
	mic->tick();
  }
}
bool Mic::audioBufferClear() {
  if (currentSample >= BUFFER_SIZE) {
	currentSample = 0;
	return true;
  }
  return false;
}
