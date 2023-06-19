//
// Created by Ewout on 16-6-2023.
//

#ifndef HAS_BIRD_RECOGNTION_SRC_M7_MFCC_H
#define HAS_BIRD_RECOGNTION_SRC_M7_MFCC_H

#include <vector>
#include "dsp/transform_functions.h"
#include "SdramAllocator.h"


//these variables are not changeable
#define FFT_SIZE 1024
#define NUM_MEL_FILTERS 48
#define NUM_DCT_OUTPUTS 32
#define NUM_HOP 512

class MFCC {
public:

  explicit MFCC() = default;
  ~MFCC();

  bool begin(uint32_t sampleRate,uint32_t sampleTime);

  float *process_audio(std::vector<float, SdramAllocator<float>> &input);

  private:
  float* output = nullptr;
  arm_mfcc_instance_f32 mfcc{};
  uint32_t outputRowsCount = 0;
#if defined(ARM_MFCC_CFFT_BASED)
  float32_t tmp[FFT_SIZE*2];
#else
  float32_t tmp[FFT_SIZE+2]{};
#endif
  void process(float32_t *input, float32_t *output);


};

#endif //HAS_BIRD_RECOGNTION_SRC_M7_MFCC_H
