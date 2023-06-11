//
// Created by Ewout on 8-6-2023.
//
#include "Mic.h"
#include "SDRAM.h"
#include "i2s.h"

#define BUFF_SIZE (16000 * sizeof(float) * 10) // 10 seconds of audio

Mic::Mic() {
  __HAL_RCC_HSEM_CLK_ENABLE();
  // Update the SystemCoreClock variable.
  SystemCoreClockUpdate();
  HAL_Init();
  MX_I2S2_Init();
}




bool Mic::begin() {
  buffer = (float *)SDRAM.malloc(BUFF_SIZE);
  if (buffer == nullptr) {
	Serial.println("SDRAM.malloc failed");
  }else{
    // print succes and amount of memory allocated
    Serial.println("SDRAM.malloc succesfull allocated\t" + String(BUFF_SIZE));
  }

  auto state = HAL_I2S_GetState(&hi2s2);
  Serial.println("HAL_I2S_GetState returned\t" + String(state));
  if(state != HAL_I2S_STATE_READY) return false;

  return true;
}

Mic::audio_buffer_t Mic::audioBufferGet() {
  return {buffer, BUFF_SIZE};
}

bool Mic::audioBufferReady() {
  return currentSample >= BUFF_SIZE;
}

#define BUFFER_SIZE UINT16_MAX
uint32_t i2sBuffer[BUFFER_SIZE]{0};
int16_t pcm16;

#define AUDIO_LEN (sizeof(i2sBuffer)/sizeof(i2sBuffer[0])) 	

void Mic::tick() {
  auto result = HAL_I2S_Receive(&hi2s2, (uint16_t *)i2sBuffer, AUDIO_LEN/2, 1000);
  if (result != HAL_OK) {
	  Serial.println("HAL_I2S_Receive failed");
  }

  for (int i = 0; i < BUFFER_SIZE; i++) {
	  pcm16 = (int16_t)(i2sBuffer[i] >> 8);
    if (pcm16 == 0) continue;
	  if (currentSample >= BUFF_SIZE) {
		  break;
	  }
    auto pcmFloat = ((float)pcm16 / (1 << 15));
	  buffer[currentSample++] = pcmFloat;
  }
  printf("currentSample: %d\n", currentSample);
}