#ifndef _PARAMS_H_
#define _PARAMS_H_

#include <stdint.h>

namespace PPA_PARAM {
  // BFV parameters
  // constexpr uint64_t PLAIN_MODULUS = 1103311814658949121ULL; // 60 bit
  // constexpr uint64_t MAX_PRIME_SIZE = ((1ULL << 60) - 1);
  constexpr uint64_t  PLAIN_MODULUS = (8088322049ULL); // 33 bit
  constexpr uint64_t MAX_PRIME_SIZE = ((1ULL << 33) - 1);
  constexpr uint64_t MOD_DEGREE =  8192;
  constexpr bool sec80 = false; // 80 bit security?

  // matrix dimensions
  constexpr uint64_t N = (1ULL << 23); // phone numbers
  constexpr uint64_t k = (1ULL << 15); // cell sites

  // Babystep giantstep multiplication
  constexpr bool USE_BSGS = true;

  // Multithreading
  constexpr uint64_t NUM_THREADS = 8;

  // Scheme security
  constexpr bool MASKING = false; // Mask matrix result
  constexpr uint64_t NUM_MASKS = 13; // Have a challenge with multiple masks
  constexpr uint64_t MIN_HW = 1024ULL; // Minimum HW of the input vector
  
  // Differential privacy
  constexpr bool DIFF_PRIV = false;
  constexpr uint64_t SENSITIVITY = UINT32_MAX;
  constexpr double EPSILON = 1.0;
}

#endif // _PARAMS_H_
