/*
* Brian R Taylor
* brian.taylor@bolderflight.com
* 
* Copyright (c) 2021 Bolder Flight Systems Inc
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the “Software”), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/

/*
* Guarantee that the Eigen code is licensed
* under the MPL2 and possibly more permissive licenses (like BSD)
*/
#define EIGEN_MPL2_ONLY

#ifdef A0
# define NEED_A0_RESTORED A0
# undef A0
#endif
#ifdef A1
# define NEED_A1_RESTORED A1
# undef A1
#endif
#ifdef B0
# define NEED_B0_RESTORED B0
# undef B0
#endif
#ifdef B1
# define NEED_B1_RESTORED B1
# undef B1
#endif
#ifdef D0
# define NEED_D0_RESTORED D0
# undef D0
#endif
#ifdef D1
# define NEED_D1_RESTORED D1
# undef D1
#endif
#ifdef D2
# define NEED_D2_RESTORED D2
# undef D2
#endif
#ifdef D3
# define NEED_D3_RESTORED D3
# undef D3
#endif

/* Disable asserts to speed up runtime */
//#define EIGEN_NO_DEBUG 1
#define EIGEN_INTERNAL_DEBUGGING 1

//#define EIGEN_NO_MALLOC 1
//#define EIGEN_NO_STATIC_ASSERT 0
#define EIGEN_NO_AUTOMATIC_RESIZING 1


#define EIGEN_SDRAM_ALLOCATOR 1
/* Include core header */
#include "Eigen/Core"
#include "Eigen/FFT"


#ifdef NEED_A0_RESTORED
# define A0 NEED_A0_RESTORED
# undef NEED_A0_RESTORED
#endif
#ifdef NEED_A1_RESTORED
# define A1 NEED_A1_RESTORED
# undef NEED_A1_RESTORED
#endif
#ifdef NEED_B0_RESTORED
# define B0 NEED_B0_RESTORED
# undef NEED_B0_RESTORED
#endif
#ifdef NEED_B1_RESTORED
# define B1 NEED_B1_RESTORED
# undef NEED_B1_RESTORED
#endif
#ifdef NEED_D0_RESTORED
# define D0 NEED_D0_RESTORED
# undef NEED_D0_RESTORED
#endif
#ifdef NEED_D1_RESTORED
# define D1 NEED_D1_RESTORED
# undef NEED_D1_RESTORED
#endif
#ifdef NEED_D2_RESTORED
# define D2 NEED_D2_RESTORED
# undef NEED_D2_RESTORED
#endif
#ifdef NEED_D3_RESTORED
# define D3 NEED_D3_RESTORED
# undef NEED_D3_RESTORED
#endif
