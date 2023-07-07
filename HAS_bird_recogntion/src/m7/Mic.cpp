//
// Created by Ewout on 8-6-2023.
//
#include "Mic.h"
#include "i2s.h"
#include "Scheduler.h"
#include "SdramAllocator.h"

#define BUFFER_SIZE ((uint32_t)(SAMPLE_RATE * SAMPLE_TIME))

Mic::Mic() {
  __HAL_RCC_HSEM_CLK_ENABLE();
  // Update the SystemCoreClock variable.
  SystemCoreClockUpdate();
  HAL_Init();
  MX_I2S2_Init();
}

bool Mic::begin() {
  buffer = FloatAllocator.allocate(BUFFER_SIZE);

  auto state = HAL_I2S_GetState(&hi2s2);
  printf("HAL_I2S_GetState returned\t%d\n", state);
  if (state != HAL_I2S_STATE_READY)
	return false;

  //start arduino thread
  Scheduler.start(thread, this, 4096);

  return true;
}

Mic::audio_buffer_t Mic::audioBufferGet() {
  return {buffer, BUFFER_SIZE};
}

bool Mic::audioBufferReady() const {
  return currentSample >= BUFFER_SIZE;
}

#define I2S_BUFFER_SIZE UINT16_MAX
uint32_t i2sBuffer[I2S_BUFFER_SIZE]{0};

void Mic::tick() {
  if (currentSample >= BUFFER_SIZE) {
	yield();
	delay(100);
	return;
  }

  auto result = HAL_I2S_Receive(&hi2s2, reinterpret_cast<uint16_t *>(i2sBuffer), I2S_BUFFER_SIZE, 1000);
  if (result != HAL_OK) {
	printf("HAL_I2S_Receive returned\t%d\n", result);
  }

  for (unsigned long value : i2sBuffer) {
	if (value == 0)
	  continue;
	//convert 24bit with the last 6 bits being 0 to 32bit signed
	auto converted = static_cast<int32_t>(value << 8);
	converted = converted >> 14;
	float max = 1 << 17;
	auto f = static_cast<float>(converted);

	//convert 32bit signed to float
	buffer[currentSample++] = (f / max);
	if (currentSample >= BUFFER_SIZE) {
	  break;
	}
  }
}

[[noreturn]] void Mic::thread(void *arg) {
  auto mic = (Mic *)arg;
  while (true) {
	mic->tick();
	yield();
  }
}
bool Mic::audioBufferClear() {
  if (currentSample >= BUFFER_SIZE) {
	currentSample = 0;
	return true;
  }
  return false;
}
