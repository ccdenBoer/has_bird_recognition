//
// Created by Ewout on 16-6-2023.
//

#ifndef HAS_BIRD_RECOGNTION_SRC_M7_MFCC_H
#define HAS_BIRD_RECOGNTION_SRC_M7_MFCC_H

#include <vector>
#include "dsp/transform_functions.h"


//these variables are not changeable
#define FFT_SIZE 512
#define NUM_MEL_FILTERS 48
#define NUM_DCT_OUTPUTS 32
#define NUM_HOP (FFT_SIZE/2)

class MFCC {
public:

  explicit MFCC() = default;
  ~MFCC();

  bool begin(uint32_t sampleRate,uint32_t sampleTime);

  float *process_audio(float* input);

  private:
  float* output = nullptr;
  float32_t* inputCopy = nullptr;
  arm_mfcc_instance_f32 mfcc{};
  uint32_t outputRowsCount = 0;
#if defined(ARM_MFCC_CFFT_BASED)
  float32_t tmp[FFT_SIZE*2];
#else
  float32_t tmp[FFT_SIZE+2]{};
#endif
  inline void process(float32_t *input, float32_t *output);
};


#endif //HAS_BIRD_RECOGNTION_SRC_M7_MFCC_H
