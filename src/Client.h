#pragma once

#include "seal/seal.h"

class Client {
 private:
  // Filenames
  static constexpr const char *SK_FILE = "sk";
  static constexpr const char *PK_FILE = "pk";
  static constexpr const char *PARAM_FILE = "param";
  static constexpr const char *GK_FILE = "gk";
  static constexpr const char *RK_FILE = "rk";
  static constexpr const char *CIPHER_FILE = "cipher_in";
  static constexpr const char *RESULT_FILE = "cipher_out";

  std::shared_ptr<seal::SEALContext> context;
  seal::KeyGenerator keygen;

  seal::GaloisKeys galois_keys;
  seal::SecretKey secret_key;
  seal::RelinKeys relin_keys;
  seal::PublicKey public_key;

  seal::Encryptor encryptor;
  seal::Evaluator evaluator;
  seal::Decryptor decryptor;
  seal::BatchEncoder encoder;

  uint64_t N;
  uint64_t k;
  uint64_t slots;
  uint64_t num_plaintexts;
  uint64_t num_ciphertexts;
  uint64_t plain_mod;

  static const std::map<size_t, std::vector<seal::Modulus>>
      default_coeff_modulus_80;

  static std::vector<seal::Modulus> BFV_80bit(size_t);

  void get_bsgs_params(uint64_t &bsgs_n1, uint64_t &bsgs_n2) const;

 public:
  static constexpr const char *RESULT_PLAIN = "plain_out";

  Client(std::shared_ptr<seal::SEALContext>, uint64_t plain_mod,
         bool to_file = false);
  Client(std::shared_ptr<seal::SEALContext>, uint64_t plain_mod,
         seal::SecretKey &);
  ~Client() = default;
  Client(const Client &) = delete;
  Client &operator=(const Client &) = delete;

  static std::shared_ptr<seal::SEALContext> create_context(
      size_t mod_degree, uint64_t plain_mod, bool sec80 = false,
      bool to_file = false);

  static std::shared_ptr<seal::SEALContext> context_from_file(bool sec80);

  int print_noise(std::vector<seal::Ciphertext> &);
  int get_noise(seal::Ciphertext &);
  void print_parameters();
  uint64_t get_num_plaintexts() const;
  uint64_t get_num_ciphertexts() const;
  uint64_t get_slots() const;

  seal::GaloisKeys &get_galois_keys();
  seal::RelinKeys &get_relin_keys();
  seal::PublicKey &get_public_key();

  void set_dimension(uint64_t N, uint64_t k);

  void create_gk(bool masking = true, bool use_bsgs = true,
                 bool to_file = false);

  bool ciphers_from_file(std::vector<seal::Ciphertext> &ciphers);

  static void sk_from_file(seal::SecretKey &,
                           std::shared_ptr<seal::SEALContext> &);

  void get_input(std::vector<uint64_t> &input);
  void get_input_from_file(std::vector<uint64_t> &input,
                           const std::string &path);
  static void write_result_to_file(std::vector<uint64_t> &res,
                                   const std::string &path);
  void encrypt(std::vector<seal::Ciphertext> &, std::vector<uint64_t> &);
  void encrypt_to_file(std::vector<uint64_t> &);
  bool decrypt(std::vector<uint64_t> &, std::vector<seal::Ciphertext> &,
               bool cap_negative = false);
  void decrypt(std::vector<uint64_t> &, seal::Ciphertext &);
};
