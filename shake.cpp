#include "shake.h"
#include "params.h"
#include "portable_endian.h"
#include <cstdio>
#include <cstdlib>
#ifdef __linux__
#include "rand.h"
#elif _WIN32
#include <wincrypt.h>
#include <windows.h>
#endif
#include <mutex>

Keccak_HashInstance shake128;
std::mutex shake_mutex;

void init_and_seed_shake() {

  uint8_t seed[16];
#ifdef __linux__
  if (getrandom(seed, 16, 0) != 16)
    perror("failed to get random seed");
#elif _WIN32
  HCRYPTPROV p;

  if (CryptAcquireContext(&p, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) ==
      FALSE) {
    perror("failed to acquire context");
  }
  if (CryptGenRandom(p, 16, (BYTE *)seed) == FALSE) {
    perror("failed to get random seed");
  }
  CryptReleaseContext(p, 0);
#else
  perror("Unknown opearating system");
#endif

  init_shake(seed, 16);
}

void init_shake(uint8_t *seed, uint64_t len) {
  if (SUCCESS != Keccak_HashInitialize_SHAKE128(&shake128))
    perror("failed to init shake");
  if (SUCCESS != Keccak_HashUpdate(&shake128, seed, len * 8))
    perror("SHAKE128 update failed");
  if (SUCCESS != Keccak_HashFinal(&shake128, NULL))
    perror("SHAKE128 final failed");
}

//----------------------------------------------------------------

uint64_t generate_random_field_element() {
  std::lock_guard<std::mutex> guard(shake_mutex);
  uint8_t random_bytes[sizeof(uint64_t)];
  while (1) {
    if (SUCCESS !=
        Keccak_HashSqueeze(&shake128, random_bytes, sizeof(random_bytes) * 8))
      perror("SHAKE128 squeeze failed");
    uint64_t ele =
        be64toh(*((uint64_t *)random_bytes)) & PPA_PARAM::MAX_PRIME_SIZE;
    if (ele < PPA_PARAM::PLAIN_MODULUS)
      return ele;
  }
}

//----------------------------------------------------------------

uint64_t generate_random_field_element_without_0() {
  while (1) {
    uint64_t ele = generate_random_field_element();
    if (ele)
      return ele;
  }
}

//----------------------------------------------------------------

uint64_t generate_random_uint64_t() {
  std::lock_guard<std::mutex> guard(shake_mutex);
  uint8_t random_bytes[sizeof(uint64_t)];
    if (SUCCESS !=
        Keccak_HashSqueeze(&shake128, random_bytes, sizeof(random_bytes) * 8))
      perror("SHAKE128 squeeze failed");
    uint64_t ele = be64toh(*((uint64_t *)random_bytes));
    return ele;
}
