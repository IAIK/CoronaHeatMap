#ifndef _PARAMS_H_
#define _PARAMS_H_

#include <stdint.h>

namespace PPA_PARAM {
// BFV parameters
// constexpr uint64_t PLAIN_MODULUS = 1103311814658949121ULL;  // 60 bit
// constexpr uint64_t MAX_PRIME_SIZE = ((1ULL << 60) - 1);
constexpr uint64_t PLAIN_MODULUS = (4398046150657ULL);  // 42 bit
constexpr uint64_t MAX_PRIME_SIZE = ((1ULL << 42) - 1);
constexpr uint64_t MOD_DEGREE = 16384;
constexpr bool sec80 = false;  // 80 bit security?

// matrix dimensions
// constexpr uint64_t N = (1ULL << 23); // phone numbers
// constexpr uint64_t k = (1ULL << 15); // cell sites
constexpr uint64_t N = MOD_DEGREE;
constexpr uint64_t k = MOD_DEGREE / 2;

// Server Mod_Switch
constexpr bool MOD_SWITCH = true;
constexpr uint64_t LEVELS_FROM_LAST = 1;
// 1 for 42 bit, 0 for 60 bit prime plain modulus

// Babystep giantstep multiplication
constexpr bool USE_BSGS = true;

// Multithreading
constexpr uint64_t NUM_THREADS = 8;

// Scheme security
constexpr bool MASKING = true;  // Mask matrix result

// circuit privacy
constexpr bool NOISE_FLOODING = true;  // perform noise flooding
constexpr uint64_t NOISE_FLOODING_BITS = 385;
// magnitude of noise for noise flooding,  368 for 60 bit, 385 for 42
// bit prime plain modulus

// Differential privacy
constexpr bool DIFF_PRIV = false;
constexpr uint64_t SENSITIVITY = UINT32_MAX;
constexpr double EPSILON = 1.0;

}  // namespace PPA_PARAM

#endif  // _PARAMS_H_
