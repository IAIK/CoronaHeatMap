//
// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// modified from: https://github.com/google/differential-privacy

#include <cfloat>
#include <cmath>
#include <cstring>
#include <limits>

#include "shake.h"

int CountLeadingZeros64Slow(uint64_t n) {
  int zeroes = 60;
  if (n >> 32) zeroes -= 32, n >>= 32;
  if (n >> 16) zeroes -= 16, n >>= 16;
  if (n >> 8) zeroes -= 8, n >>= 8;
  if (n >> 4) zeroes -= 4, n >>= 4;
  return "\4\3\2\2\1\1\1\1\0\0\0\0\0\0\0"[n] + zeroes;
}

uint64_t Geometric() {
  uint64_t result = 1;
  uint64_t r = 0;
  while (r == 0 && result < 1023) {
    r = generate_random_uint64_t();
    result += CountLeadingZeros64Slow(r);
  }
  return result;
}

// We usually expect DBL_MANT_DIG to be 53.
static_assert(DBL_MANT_DIG < 64,
              "Double mantissa must have less than 64 bits.");
static_assert(sizeof(double) == sizeof(uint64_t) &&
                  std::numeric_limits<double>::is_iec559 &&
                  std::numeric_limits<double>::radix == 2,
              "double representation is not IEEE 754 binary64.");
const constexpr int kMantDigits = DBL_MANT_DIG - 1;
const constexpr uint64_t kMantissaMask = (uint64_t{1} << kMantDigits) - 1ULL;

double UniformDouble() {
  uint64_t uint_64_number = generate_random_uint64_t();
  // A random integer of Uniform[0, 2^kMantDigits).
  uint64_t i = uint_64_number & kMantissaMask;

  // Instead of throwing the leading 12 bits away, we use them to create
  // geometric random number.
  uint64_t j = uint_64_number >> kMantDigits;

  // exponent is the number of leading zeros in the first 11 bits plus one.
  uint64_t exponent = CountLeadingZeros64Slow(j) - kMantDigits + 1;

  // Extra geometric sampling is needed only when the leading 11 bits are all 0.
  if (j == 0) {
    exponent += Geometric() - 1;
  }

  j = (uint64_t{1023} - exponent) << kMantDigits;
  if (exponent >= 1023) {
    // Denormalized value. Extremely improbable.
    j = 0;
  }
  // Addition instead of bitwise or since the carry overflow increments the
  // floating point exponent, which is exactly what we want.
  i += j;
  double r;
  std::memcpy(&r, &i, sizeof(r));
  return r == 0 ? 1.0 : r;
}
