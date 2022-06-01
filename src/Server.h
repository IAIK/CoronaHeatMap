#pragma once

#include "seal/seal.h"

class Server {
 private:
  // Filenames
  static constexpr const char *PARAM_FILE = "param";
  static constexpr const char *GK_FILE = "gk";
  static constexpr const char *PK_FILE = "pk";
  static constexpr const char *HW_FILE = "hw";
  static constexpr const char *RK_FILE = "rk";
  static constexpr const char *CIPHER_FILE = "cipher_in";
  static constexpr const char *RESULT_FILE = "cipher_out";

  typedef std::vector<std::vector<uint32_t>> plain_matrix;

  std::vector<seal::Ciphertext> input;
  bool in_set = false;

  std::shared_ptr<seal::SEALContext> context;
  seal::Evaluator evaluator;
  seal::BatchEncoder encoder;

  seal::GaloisKeys galois_keys;
  seal::RelinKeys relin_keys;
  seal::PublicKey public_key;

  uint64_t plain_mod;

  uint64_t N;
  uint64_t k;
  uint64_t slots;
  uint64_t num_plaintexts;
  uint64_t num_ciphertexts;

  std::atomic_uint64_t counter;

  uint64_t num_threads = 1;

  bool diff_priv = false;
  uint64_t sensitivity;
  double epsilon;

  bool noise_flooding = false;
  uint64_t noise_bits = 0;

  bool mod_switch = false;
  uint64_t mod_switch_levels_from_last = 0;

  bool use_bsgs;
  bool gk_set = false;
  bool relin_set = false;
  bool pk_set = false;

  uint64_t bsgs_n1;
  uint64_t bsgs_n2;

  void set_bsgs_params();

  void sum_elements(seal::Ciphertext &);
  void dot(const seal::Ciphertext &, seal::Ciphertext &);
  void minus_1(const std::vector<seal::Ciphertext> &,
               std::vector<seal::Ciphertext> &);

  void computePartBinMask(seal::Ciphertext &,
                          const std::vector<seal::Ciphertext> &,
                          const std::vector<seal::Ciphertext> &);
  void computeBinMask(seal::Ciphertext &,
                      const std::vector<seal::Ciphertext> &);

  void diagonal(seal::Ciphertext &state, const plain_matrix &mat);
  void babystep_giantstep(seal::Ciphertext &state, const plain_matrix &mat);

  bool compute_internal(std::vector<seal::Ciphertext> &out,
                        bool masking = false);

  static void runner(std::vector<seal::Ciphertext> &,
                     const std::vector<seal::Ciphertext> &, uint64_t,
                     uint64_t num, Server &);

  void get_submatrix(plain_matrix &, uint64_t, uint64_t);
  inline uint32_t get_matrix_element(uint64_t row, uint64_t col);

  static void runner_plain(std::vector<uint64_t> &,
                           const std::vector<uint64_t> &, uint64_t, uint64_t,
                           Server &);

 public:
  static constexpr const bool VERBOSE = false;

  Server(std::shared_ptr<seal::SEALContext> context, uint64_t plain_mod,
         bool use_bsgs = true);
  Server();
  ~Server() = default;
  Server(const Server &) = delete;
  Server &operator=(const Server &) = delete;

  static std::shared_ptr<seal::SEALContext> context_from_file(bool sec80);

  void keys_from_file(bool masking, bool noise_flooding);

  void print_parameters();

  void set_gk(seal::GaloisKeys &);
  void set_rk(seal::RelinKeys &);
  void set_pk(seal::PublicKey &);

  void set_dimension(uint64_t N, uint64_t k);

  void set_num_threads(uint64_t);

  bool compute(std::vector<seal::Ciphertext> &out);

  bool computeWithMask(std::vector<seal::Ciphertext> &out);

  void computeMask(std::vector<seal::Ciphertext> &mask);

  void set_input(std::vector<seal::Ciphertext> &);

  void compute_plain(std::vector<uint64_t> &out,
                     const std::vector<uint64_t> &in);

  bool ciphers_from_file();
  void ciphers_to_file(std::vector<seal::Ciphertext> &ciphers);

  void activate_diff_priv(bool activate, u_int64_t sensitivity, double epsilon);
  double laplace() const;
  int64_t laplace_round() const;

  void activate_noise_flooding(bool activate, uint64_t noise_bits);

  void activate_mod_switch(bool activate, uint64_t levels_from_last);
  inline void increment_counter();
};
