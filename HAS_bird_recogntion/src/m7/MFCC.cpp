//
// Created by Ewout on 16-6-2023.
//

#include "MFCC.h"
#include "mfccdata.h"
#include <cstdio>


extern "C"{
    arm_mfcc_instance_f32 mfcc_iternal;
}



MFCC::MFCC(){

}

void MFCC::process(float32_t *input, float32_t *output) {
//  arm_mfcc_f32(&mfcc, input, output, tmp);
}

bool MFCC::begin() {
    auto status = arm_mfcc_init_1024_f32(&mfcc_iternal, NUM_MEL_FILTERS, NUM_DCT_OUTPUTS,
                                         mfcc_dct_coefs_config1_f32,
                                         mfcc_filter_pos_config1_f32, mfcc_filter_len_config1_f32,
                                         mfcc_filter_coefs_config1_f32,
                                         mfcc_window_coefs_config1_f32);

    if (status != ARM_MATH_SUCCESS) {
        printf("MFCC init failed\n");
        return false;
    }
    printf("MFCC init success\n");
    return true;
}


float *MFCC::process_audio(std::vector<float, SdramAllocator<float>> &input) {
  auto samplesCount = input.size();
  auto outputRows = (samplesCount - FFT_SIZE) / NUM_HOP;
  size_t size = outputRows * NUM_DCT_OUTPUTS * sizeof(float);
  float* output = FloatAllocator.allocate(size);
  //output shape is float[outputRows][NUM_DCT_OUTPUTS]

  for (size_t i = 0; i < outputRows; i++) {
	process(&input[i * NUM_HOP], &output[i * NUM_DCT_OUTPUTS]);
  }

  return output;
}


