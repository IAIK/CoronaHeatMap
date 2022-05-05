#include <seal/seal.h>

void encrypt_zero_noise_flooding(
    seal::Ciphertext &destination, seal::SEALContext &context,
    seal::PublicKey &public_key, uint64_t noise_bits,
    seal::MemoryPoolHandle pool = seal::MemoryManager::GetPool());
