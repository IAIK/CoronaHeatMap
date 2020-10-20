#ifndef _PARAMS_H_
#define _PARAMS_H_

#include <stdint.h>

namespace PPA_PARAM {
  // BFV parameters
  // constexpr uint64_t PLAIN_MODULUS = 1103311814658949121ULL; // 60 bit
  // constexpr uint64_t MAX_PRIME_SIZE = ((1ULL << 60) - 1);
  // constexpr uint64_t PLAIN_MODULUS = (4398046150657ULL); // 42 bit
  // constexpr uint64_t MAX_PRIME_SIZE = ((1ULL << 42) - 1);
  constexpr uint64_t PLAIN_MODULUS = (8088322049ULL); // 33 bit
  constexpr uint64_t MAX_PRIME_SIZE = ((1ULL << 33) - 1);
  constexpr uint64_t MOD_DEGREE = 8192;
  constexpr bool sec80 = false; // 80 bit security?

  // Server Mod_Switch
  constexpr bool MOD_SWITCH = true;
  constexpr uint64_t LEVELS_FROM_LAST = 0;

  // matrix dimensions
  // constexpr uint64_t N = (1ULL << 23); // phone numbers
  // constexpr uint64_t k = (1ULL << 15); // cell sites
  constexpr uint64_t N = MOD_DEGREE;
  constexpr uint64_t k = MOD_DEGREE / 2;

  // Babystep giantstep multiplication
  constexpr bool USE_BSGS = true;

  // Multithreading
  constexpr uint64_t NUM_THREADS = 8;

  // Scheme security
  constexpr bool MASKING = true; // Mask matrix result
  constexpr uint64_t MIN_HW = 1024ULL; // Minimum HW of the input vector

  // Differential privacy
  constexpr bool DIFF_PRIV = false;
  constexpr uint64_t SENSITIVITY = UINT32_MAX;
  constexpr double EPSILON = 1.0;

  // Testing
  constexpr bool RANDOM_MATRIX = true;

} // namespace PPA_PARAM

#endif // _PARAMS_H_
