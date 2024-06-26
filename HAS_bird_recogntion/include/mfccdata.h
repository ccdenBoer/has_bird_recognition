//!!! created automatically by GenMFCCDataForCPP.py
//!!! DO NOT EDIT THIS FILE MANUALLY

/// settings used to generate this file:
// dct:
//  config1_f32:
//    melFilters: 48
//    dctOutputs: 32
//    type: "f32"
// melfilter:
//  config1_f32:
//    fftlength: 512
//    fmin: 64
//    fmax: 8000
//    samplingRate : 16000
//    melFilters: 48
//    type: "f32"
// window:
//  config1_f32:
//    fftlength: 512
//    type: "f32"
//    win: "hamming"

#ifndef _MFCC_DATA_H_
#define _MFCC_DATA_H_

#include "arm_math_types.h"


#ifdef   __cplusplus
extern "C"
{
#endif


/*****

 DCT COEFFICIENTS FOR THE MFCC

*****/


#define NB_MFCC_DCT_COEFS_CONFIG1_F32 1536
extern const float32_t mfcc_dct_coefs_config1_f32[NB_MFCC_DCT_COEFS_CONFIG1_F32];



/*****

 WINDOW COEFFICIENTS

*****/


#define NB_MFCC_WIN_COEFS_CONFIG1_F32 512
extern const float32_t mfcc_window_coefs_config1_f32[NB_MFCC_WIN_COEFS_CONFIG1_F32];



/*****

 MEL FILTER COEFFICIENTS FOR THE MFCC

*****/

#define NB_MFCC_NB_FILTER_CONFIG1_F32 48
extern const uint32_t mfcc_filter_pos_config1_f32[NB_MFCC_NB_FILTER_CONFIG1_F32];
extern const uint32_t mfcc_filter_len_config1_f32[NB_MFCC_NB_FILTER_CONFIG1_F32];





#define NB_MFCC_FILTER_COEFS_CONFIG1_F32 492
extern const float32_t mfcc_filter_coefs_config1_f32[NB_MFCC_FILTER_COEFS_CONFIG1_F32];



#ifdef   __cplusplus
}
#endif

#endif

