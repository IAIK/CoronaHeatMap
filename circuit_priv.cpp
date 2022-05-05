#include "circuit_priv.h"

#include <seal/seal.h>
#include <seal/util/clipnormal.h>
#include <seal/util/defines.h>
#include <seal/util/polyarithsmallmod.h>
#include <seal/util/rlwe.h>

#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

namespace {

uint64_t generate_random_uint64_t(
    std::shared_ptr<seal::UniformRandomGenerator> &prng) {
  uint64_t low, high;
  low = prng->generate();
  high = prng->generate();
  return low + (high << 32);
}

bool generate_random_bool(std::shared_ptr<seal::UniformRandomGenerator> &prng) {
  seal::seal_byte byte;
  prng->generate(1, &byte);
  return (std::to_integer<int>(byte) & 1) == 1;
}

class RandBigInt {
 private:
  std::vector<uint64_t> limbs_;
  bool is_neg_;

 public:
  RandBigInt(size_t bits, std::shared_ptr<seal::UniformRandomGenerator> &prng);
  ~RandBigInt() = default;
  RandBigInt(const RandBigInt &) = delete;
  RandBigInt &operator=(const RandBigInt &) = delete;

  uint64_t mod_limb(uint64_t mod) const;
  bool is_neg() const { return this->is_neg_; }
  void print() const;
};

RandBigInt::RandBigInt(size_t num_bits,
                       std::shared_ptr<seal::UniformRandomGenerator> &prng) {
  num_bits--;  // is_neg is sampled separately
  size_t num_limbs = ceil((double)num_bits / 64);
  limbs_.reserve(num_limbs);

  while (num_bits > 0) {
    if (num_bits < 64) {
      uint64_t mask = (1ULL << num_bits) - 1;
      uint64_t rand = generate_random_uint64_t(prng) & mask;
      limbs_.push_back(rand);
      num_bits = 0;
    } else {
      limbs_.push_back(generate_random_uint64_t(prng));
      num_bits -= 64;
    }
  }
  is_neg_ = generate_random_bool(prng);
}

uint64_t RandBigInt::mod_limb(uint64_t mod) const {
  size_t num_limbs = limbs_.size();
  if (num_limbs == 0) return 0;

  __uint128_t result = limbs_[0] % mod;
  if (num_limbs == 1) return (uint64_t)result;

  __uint128_t two_64 = 1ULL;
  two_64 <<= 64;
  uint64_t two_64_red = two_64 % mod;

  uint64_t powers = two_64_red;

  for (size_t i = 1; i < num_limbs; i++) {
    uint64_t tmp = ((__uint128_t)powers * limbs_[i]) % mod;
    result = (result + tmp) % mod;
    if (i != num_limbs - 1) {
      powers = ((__uint128_t)powers * two_64_red) % mod;
    }
  }

  return (uint64_t)result;
}

[[maybe_unused]] void RandBigInt::print() const {
  std::ios_base::fmtflags f(std::cout.flags());  // get flags
  if (is_neg()) std::cout << "-";
  std::cout << "0x";
  for (auto it = limbs_.rbegin(); it != limbs_.rend(); ++it) {
    std::cout << std::hex << std::setw(16) << std::setfill('0');
    std::cout << *it;
  }
  std::cout << std::endl;

  std::cout.flags(f);  // reset flags
}

void sample_poly_large_uniform(
    std::shared_ptr<seal::UniformRandomGenerator> prng,
    const seal::EncryptionParameters &parms, uint64_t *destination,
    uint64_t noise_bits) {
  auto coeff_modulus = parms.coeff_modulus();
  size_t coeff_modulus_size = coeff_modulus.size();
  size_t coeff_count = parms.poly_modulus_degree();

  SEAL_ITERATE(seal::util::iter(destination), coeff_count, [&](auto &I) {
    RandBigInt noise(noise_bits, prng);
    uint64_t flag =
        static_cast<uint64_t>(-static_cast<int64_t>(noise.is_neg()));
    SEAL_ITERATE(
        seal::util::iter(seal::util::StrideIter<uint64_t *>(&I, coeff_count),
                         coeff_modulus),
        coeff_modulus_size, [&](auto J) {
          int64_t rem =
              static_cast<int64_t>(noise.mod_limb(std::get<1>(J).value()));
          if (noise.is_neg()) rem *= -1;
          *std::get<0>(J) =
              static_cast<uint64_t>(rem) + (flag & std::get<1>(J).value());
        });
  });
}

void encrypt_zero_asymmetric_noise_flooding(const seal::PublicKey &public_key,
                                            const seal::SEALContext &context,
                                            seal::parms_id_type parms_id,
                                            bool is_ntt_form,
                                            uint64_t noise_bits,
                                            seal::Ciphertext &destination) {
  // We use a fresh memory pool with `clear_on_destruction' enabled
  seal::MemoryPoolHandle pool =
      seal::MemoryManager::GetPool(seal::mm_prof_opt::mm_force_new, true);

  auto &context_data = *context.get_context_data(parms_id);
  auto &parms = context_data.parms();
  auto &coeff_modulus = parms.coeff_modulus();
  size_t coeff_modulus_size = coeff_modulus.size();
  size_t coeff_count = parms.poly_modulus_degree();
  auto ntt_tables = context_data.small_ntt_tables();
  size_t encrypted_size = public_key.data().size();

  // Make destination have right size and parms_id
  // Ciphertext (c_0,c_1, ...)
  destination.resize(context, parms_id, encrypted_size);
  destination.is_ntt_form() = is_ntt_form;
  destination.scale() = 1.0;

  // c[j] = public_key[j] * u + e[j] where e[j] <-- chi, u <-- R_3

  // Create a PRNG; u and the noise/error share the same PRNG
  auto prng = parms.random_generator()->create();

  // Generate u <-- R_3
  auto u(seal::util::allocate_poly(coeff_count, coeff_modulus_size, pool));
  seal::util::sample_poly_ternary(prng, parms, u.get());

  // c[j] = u * public_key[j]
  for (size_t i = 0; i < coeff_modulus_size; i++) {
    ntt_negacyclic_harvey(u.get() + i * coeff_count, ntt_tables[i]);
    for (size_t j = 0; j < encrypted_size; j++) {
      seal::util::dyadic_product_coeffmod(
          u.get() + i * coeff_count,
          public_key.data().data(j) + i * coeff_count, coeff_count,
          coeff_modulus[i], destination.data(j) + i * coeff_count);

      // Addition with e_0, e_1 is in non-NTT form
      if (!is_ntt_form) {
        inverse_ntt_negacyclic_harvey(destination.data(j) + i * coeff_count,
                                      ntt_tables[i]);
      }
    }
  }

  // Generate e_j <-- chi
  // c[j] = public_key[j] * u + e[j]
  for (size_t j = 0; j < encrypted_size; j++) {
    sample_poly_large_uniform(prng, parms, u.get(), noise_bits);
    for (size_t i = 0; i < coeff_modulus_size; i++) {
      // Addition with e_0, e_1 is in NTT form
      if (is_ntt_form) {
        ntt_negacyclic_harvey(u.get() + i * coeff_count, ntt_tables[i]);
      }
      seal::util::add_poly_coeffmod(
          u.get() + i * coeff_count, destination.data(j) + i * coeff_count,
          coeff_count, coeff_modulus[i], destination.data(j) + i * coeff_count);
    }
  }
}

void encrypt_zero_internal_noise_flooding(seal::parms_id_type parms_id,
                                          seal::Ciphertext &destination,
                                          seal::SEALContext &context,
                                          seal::PublicKey &public_key,
                                          uint64_t noise_bits,
                                          seal::MemoryPoolHandle pool) {
  // Verify parameters.
  if (!pool) {
    throw std::invalid_argument("pool is uninitialized");
  }

  auto context_data_ptr = context.get_context_data(parms_id);
  if (!context_data_ptr) {
    throw std::invalid_argument(
        "parms_id is not valid for encryption parameters");
  }

  auto &context_data = *context.get_context_data(parms_id);
  auto &parms = context_data.parms();
  size_t coeff_modulus_size = parms.coeff_modulus().size();
  size_t coeff_count = parms.poly_modulus_degree();
  bool is_ntt_form = false;

  if (parms.scheme() != seal::scheme_type::bfv) {
    throw std::invalid_argument("unsupported scheme");
  }

  // Resize destination and save results
  destination.resize(context, parms_id, 2);

  auto prev_context_data_ptr = context_data.prev_context_data();
  if (prev_context_data_ptr) {
    // Requires modulus switching
    auto &prev_context_data = *prev_context_data_ptr;
    auto &prev_parms_id = prev_context_data.parms_id();
    auto rns_tool = prev_context_data.rns_tool();

    // Zero encryption without modulus switching
    seal::Ciphertext temp(pool);
    encrypt_zero_asymmetric_noise_flooding(public_key, context, prev_parms_id,
                                           is_ntt_form, noise_bits, temp);

    // Modulus switching
    SEAL_ITERATE(seal::util::iter(temp, destination), temp.size(), [&](auto I) {
      if (is_ntt_form) {
        rns_tool->divide_and_round_q_last_ntt_inplace(
            std::get<0>(I), prev_context_data.small_ntt_tables(), pool);
      } else {
        rns_tool->divide_and_round_q_last_inplace(std::get<0>(I), pool);
      }
      set_poly(std::get<0>(I), coeff_count, coeff_modulus_size, std::get<1>(I));
    });

    destination.is_ntt_form() = is_ntt_form;
    destination.scale() = temp.scale();
    destination.parms_id() = parms_id;
  } else {
    // Does not require modulus switching
    encrypt_zero_asymmetric_noise_flooding(
        public_key, context, parms_id, is_ntt_form, noise_bits, destination);
  }
}
}  // namespace

void encrypt_zero_noise_flooding(seal::Ciphertext &destination,
                                 seal::SEALContext &context,
                                 seal::PublicKey &public_key,
                                 uint64_t noise_bits,
                                 seal::MemoryPoolHandle pool) {
  encrypt_zero_internal_noise_flooding(context.first_parms_id(), destination,
                                       context, public_key, noise_bits, pool);
}
