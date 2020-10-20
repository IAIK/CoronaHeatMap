#ifndef _SHAKE_H_
#define _SHAKE_H_

#include <cstdint>

extern "C" {
  #include "KeccakHash.h"
}

extern Keccak_HashInstance shake128;
void init_and_seed_shake();
void init_shake(uint8_t* seed, uint64_t len);
uint64_t generate_random_field_element_without_0();
uint64_t generate_random_field_element();
uint64_t generate_random_uint64_t();
double UniformDouble();

#endif // _SHAKE_H_
