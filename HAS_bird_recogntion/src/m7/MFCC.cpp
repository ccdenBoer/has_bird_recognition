//
// Created by Ewout on 16-6-2023.
//

#include "MFCC.h"
#include "mfccdata.h"
#include <cstdio>


void MFCC::process(float32_t *input, float32_t *outputSlice) {
  arm_mfcc_f32(&mfcc, input, outputSlice, tmp);
}

bool MFCC::begin(uint32_t sampleRate,uint32_t sampleTime) {
    auto status = arm_mfcc_init_1024_f32(&mfcc, NUM_MEL_FILTERS, NUM_DCT_OUTPUTS,
                                         mfcc_dct_coefs_config1_f32,
                                         mfcc_filter_pos_config1_f32, mfcc_filter_len_config1_f32,
                                         mfcc_filter_coefs_config1_f32,
                                         mfcc_window_coefs_config1_f32);

    if (status != ARM_MATH_SUCCESS) {
        printf("MFCC init failed\n");
        return false;
    }
    printf("MFCC init success\n");
  auto samplesCount = sampleRate * sampleTime;
  this->outputRowsCount = (samplesCount - FFT_SIZE) / NUM_HOP;
  size_t size = outputRowsCount * NUM_DCT_OUTPUTS;
  output = FloatAllocator.allocate(size);
  if (output == nullptr) {
	printf("MFCC output allocation failed\n");
	return false;
  }
  //print size of output in bytes
  printf("MFCC output allocation success: %d\n", size * sizeof(float));

  return true;
}


float *MFCC::process_audio(std::vector<float, SdramAllocator<float>> &input) {
  float* inputPtr = input.data();
  for (size_t i = 0; i < outputRowsCount; i++) {
	process(&inputPtr[i * NUM_HOP], &output[i * NUM_DCT_OUTPUTS]);
  }

  //print last 32 values
  for (size_t i = outputRowsCount - NUM_DCT_OUTPUTS; i < outputRowsCount; i++) {
	printf("%f ", output[outputRowsCount * NUM_DCT_OUTPUTS - NUM_DCT_OUTPUTS + i]);
  }
  printf("\n");
  return output;
}

MFCC::~MFCC() {
  if (output != nullptr) {
	FloatAllocator.deallocate(output, outputRowsCount * NUM_DCT_OUTPUTS);
  }
}

