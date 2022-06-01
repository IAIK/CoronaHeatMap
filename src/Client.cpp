#include "Client.h"

#include <assert.h>

#include <fstream>
#include <iostream>

using namespace seal;

//----------------------------------------------------------------

const std::map<size_t, std::vector<seal::Modulus>>
    Client::default_coeff_modulus_80{
        /*
        Polynomial modulus: 1x^4096 + 1
        Modulus count: 3
        Total bit count: 162 = 3 * 54
        */
        {4096, {0x3ffffffffd6001, 0x3ffffffffd2001, 0x3ffffffffbe001}},

        /*
        Polynomial modulus: 1x^8192 + 1
        Modulus count: 7
        Total bit count: 329 = 7 * 47
        */
        {8192,
         {0x7ffffffec001, 0x7ffffffc8001, 0x7ffffffb4001, 0x7ffffff00001,
          0x7fffffefc001, 0x7fffffecc001, 0x7fffffe70001}}};

//----------------------------------------------------------------

Client::Client(std::shared_ptr<seal::SEALContext> con, uint64_t p_mod,
               bool to_file)
    : context(con),
      keygen(*context),
      secret_key(keygen.secret_key()),
      encryptor(*context, secret_key),
      evaluator(*context),
      decryptor(*context, secret_key),
      encoder(*context),
      slots(encoder.slot_count()),
      plain_mod(p_mod) {
  if (to_file) {
    std::ofstream rk;
    rk.open(RK_FILE);
    auto rlk = keygen.create_relin_keys();
    rlk.save(rk);

    std::ofstream sk;
    sk.open(SK_FILE);
    secret_key.save(sk);

    std::ofstream pk;
    pk.open(PK_FILE);
    auto pubkey = keygen.create_public_key();
    pubkey.save(pk);
  }
  keygen.create_relin_keys(relin_keys);
  keygen.create_public_key(public_key);
}

//----------------------------------------------------------------

Client::Client(std::shared_ptr<seal::SEALContext> con, uint64_t p_mod,
               seal::SecretKey &sk)
    : context(con),
      keygen(*context),
      secret_key(sk),
      encryptor(*context, secret_key),
      evaluator(*context),
      decryptor(*context, secret_key),
      encoder(*context),
      slots(encoder.slot_count()),
      plain_mod(p_mod) {
  keygen.create_relin_keys(relin_keys);
  keygen.create_public_key(public_key);
}

//----------------------------------------------------------------

void Client::sk_from_file(seal::SecretKey &seckey,
                          std::shared_ptr<seal::SEALContext> &context) {
  std::ifstream sk;
  sk.open(SK_FILE);
  seckey.load(*context, sk);
}

//----------------------------------------------------------------

bool Client::ciphers_from_file(std::vector<seal::Ciphertext> &ciphs) {
  ciphs.reserve(num_ciphertexts);
  std::ifstream c;
  c.open(RESULT_FILE);
  try {
    while (true) {
      Ciphertext ct;
      ct.load(*context, c);
      ciphs.push_back(ct);
    }
  } catch (std::exception &e) {
  }
  std::cout << "...done" << std::endl;

  return (ciphs.size() == num_ciphertexts);
}

//----------------------------------------------------------------

std::shared_ptr<seal::SEALContext> Client::create_context(size_t mod_degree,
                                                          uint64_t plain_mod,
                                                          bool sec80,
                                                          bool to_file) {
  EncryptionParameters parms(scheme_type::bfv);
  parms.set_poly_modulus_degree(mod_degree);
  sec_level_type sec = sec_level_type::tc128;
  if (sec80) {
    sec = sec_level_type::none;
    parms.set_coeff_modulus(BFV_80bit(mod_degree));
  } else {
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(mod_degree));
  }
  parms.set_plain_modulus(plain_mod);

  if (to_file) {
    std::ofstream param;
    param.open(PARAM_FILE);
    parms.save(param);
  }

  return std::make_shared<seal::SEALContext>(parms, true, sec);
}

//----------------------------------------------------------------

std::shared_ptr<seal::SEALContext> Client::context_from_file(bool sec80) {
  EncryptionParameters parms;

  std::ifstream param;
  param.open(PARAM_FILE);
  parms.load(param);

  sec_level_type sec = sec_level_type::tc128;
  if (sec80) {
    sec = sec_level_type::none;
  }
  return std::make_shared<seal::SEALContext>(parms, true, sec);
}

//----------------------------------------------------------------

int Client::print_noise(std::vector<Ciphertext> &ciphs) {
  int min = decryptor.invariant_noise_budget(ciphs[0]);
  int max = min;
  for (uint64_t i = 1; i < ciphs.size(); i++) {
    int budget = decryptor.invariant_noise_budget(ciphs[i]);
    if (budget > max) max = budget;
    if (budget < min) min = budget;
  }
  std::cout << "min noise budget: " << min << std::endl;
  std::cout << "max noise budget: " << max << std::endl;
  return min;
}

//----------------------------------------------------------------

void Client::print_parameters() {
  // Verify parameters
  if (!context) {
    throw std::invalid_argument("context is not set");
  }
  auto &context_data = *context->key_context_data();

  /*
  Which scheme are we using?
  */
  std::string scheme_name;
  switch (context_data.parms().scheme()) {
    case scheme_type::bfv:
      scheme_name = "BFV";
      break;
    case scheme_type::ckks:
      scheme_name = "CKKS";
      break;
    default:
      throw std::invalid_argument("unsupported scheme");
  }
  std::cout << "/" << std::endl;
  std::cout << "| Encryption parameters:" << std::endl;
  std::cout << "|   scheme: " << scheme_name << std::endl;
  std::cout << "|   poly_modulus_degree: "
            << context_data.parms().poly_modulus_degree() << std::endl;

  /*
  Print the size of the true (product) coefficient modulus.
  */
  std::cout << "|   coeff_modulus size: ";
  std::cout << context_data.total_coeff_modulus_bit_count() << " (";
  auto coeff_modulus = context_data.parms().coeff_modulus();
  std::size_t coeff_mod_count = coeff_modulus.size();
  for (std::size_t i = 0; i < coeff_mod_count - 1; i++) {
    std::cout << coeff_modulus[i].bit_count() << " + ";
  }
  std::cout << coeff_modulus.back().bit_count();
  std::cout << ") bits" << std::endl;

  /*
  For the BFV scheme print the plain_modulus parameter.
  */
  if (context_data.parms().scheme() == scheme_type::bfv) {
    std::cout << "|   plain_modulus: "
              << context_data.parms().plain_modulus().value() << std::endl;
  }

  std::cout << "\\" << std::endl;
}

//----------------------------------------------------------------

void Client::set_dimension(uint64_t N_in, uint64_t k_in) {
  this->N = N_in;
  this->k = k_in;

  num_plaintexts = ceil((double)N / slots);
  num_ciphertexts = ceil(2 * (double)k / slots);
}

//----------------------------------------------------------------

void Client::create_gk(bool masking, bool use_bsgs, bool to_file) {
  // key for rotations, only create those needed
  std::vector<int> gks;
  gks.push_back(0);
  gks.push_back(1);
  if (use_bsgs) {
    uint64_t bsgs_n1;
    uint64_t bsgs_n2;
    get_bsgs_params(bsgs_n1, bsgs_n2);
    for (uint64_t l = 1; l < bsgs_n2; l++) gks.push_back(l * bsgs_n1);
  }
  if (masking) {
    uint64_t rot_index = 2;
    while (rot_index < (slots >> 1)) {
      gks.push_back(rot_index);
      rot_index *= 2;
    }
  }
  if (to_file) {
    std::ofstream gk;
    gk.open(GK_FILE);
    auto glk = keygen.create_galois_keys(gks);
    glk.save(gk);
  } else
    keygen.create_galois_keys(gks, galois_keys);
}

//----------------------------------------------------------------

std::vector<seal::Modulus> Client::BFV_80bit(size_t poly_modulus_degree) {
  return default_coeff_modulus_80.at(poly_modulus_degree);
}

//----------------------------------------------------------------

uint64_t Client::get_num_plaintexts() const { return num_plaintexts; }

//----------------------------------------------------------------

uint64_t Client::get_num_ciphertexts() const { return num_ciphertexts; }

//----------------------------------------------------------------

uint64_t Client::get_slots() const { return slots; }

//----------------------------------------------------------------

seal::GaloisKeys &Client::get_galois_keys() { return galois_keys; }

//----------------------------------------------------------------

seal::RelinKeys &Client::get_relin_keys() { return relin_keys; }

//----------------------------------------------------------------

seal::PublicKey &Client::get_public_key() { return public_key; }

//----------------------------------------------------------------

void Client::get_bsgs_params(uint64_t &bsgs_n1, uint64_t &bsgs_n2) const {
  uint64_t deg = ceil(log2((double)(slots >> 1)));
  uint64_t deg1 = deg >> 1;
  if (deg1 * 2 != deg) deg1++;
  uint64_t deg2 = deg - deg1;
  bsgs_n1 = (1ULL << deg1);
  bsgs_n2 = (1ULL << deg2);
  assert((slots >> 1) == bsgs_n1 * bsgs_n2);
}

//----------------------------------------------------------------

// TODO read input for real data, for now just random
void Client::get_input(std::vector<uint64_t> &input) {
  input.reserve(N);
  srand(0);
  for (uint64_t i = 0; i < N; i++) {
    int r = rand();
    r = (r > RAND_MAX / 2) ? 1 : 0;
    input.push_back(r);
  }
}

void Client::get_input_from_file(std::vector<uint64_t> &input,
                                 const std::string &path) {
  std::ifstream is;
  is.open(path);
  if (!is.is_open()) {
    std::cout << "ERROR: Unable to open file: " << path << std::endl;
    exit(-1);
  }
  std::string in;
  int intermediate;
  while (!is.eof()) {
    std::getline(is, in, ',');
    intermediate = std::stoi(in);
    if (intermediate != 0 && intermediate != 1) {
      std::cout << "ERROR: csv format incorrect: " << intermediate << std::endl;
      exit(-1);
    }
    input.push_back(intermediate);
  }
  std::cout << "size: " << input.size() << std::endl;
  std::cout << "N " << N << std::endl;
}

void Client::write_result_to_file(std::vector<uint64_t> &res,
                                  const std::string &path) {
  std::ofstream os;
  os.open(path);
  if (!os.is_open()) {
    std::cout << "ERROR: Unable to open file: " << path << std::endl;
    exit(-1);
  }
  for (uint64_t i = 0; i < res.size(); i++) {
    if (i != 0) {
      os << ",";
    }
    os << std::to_string(res[i]);
  }
  os.close();
}

//----------------------------------------------------------------
void Client::encrypt(std::vector<Ciphertext> &ciphs,
                     std::vector<uint64_t> &input) {
  std::vector<uint64_t> plain;
  ciphs.reserve(num_plaintexts);

  for (uint64_t i = 0; i < num_plaintexts; i++) {
    plain.clear();
    plain.reserve(slots);
    for (uint64_t j = 0; j < slots && i * slots + j < N; j++) {
      uint64_t val = input[i * slots + j];
      plain.push_back(val);
    }
    plain.resize(slots, 0);  // pad with zero
    Plaintext plain_enc;
    encoder.encode(plain, plain_enc);
    Ciphertext ciph;
    encryptor.encrypt_symmetric(plain_enc, ciph);
    ciphs.push_back(ciph);
  }
}

//----------------------------------------------------------------
void Client::encrypt_to_file(std::vector<uint64_t> &input) {
  std::ofstream c;
  c.open(CIPHER_FILE);
  std::vector<uint64_t> plain;

  for (uint64_t i = 0; i < num_plaintexts; i++) {
    plain.clear();
    plain.reserve(slots);
    for (uint64_t j = 0; j < slots && i * slots + j < N; j++) {
      uint64_t val = input[i * slots + j];
      plain.push_back(val);
    }
    plain.resize(slots, 0);  // pad with zero
    Plaintext plain_enc;
    encoder.encode(plain, plain_enc);
    auto ct = encryptor.encrypt_symmetric(plain_enc);
    ct.save(c);
  }
}

//----------------------------------------------------------------

bool Client::decrypt(std::vector<uint64_t> &res, std::vector<Ciphertext> &ciphs,
                     bool cap_negative) {
  if (ciphs.size() != num_ciphertexts) return false;
  res.reserve(k);
  for (uint64_t i = 0; i < num_ciphertexts; i++) {
    Plaintext ptxt;
    decryptor.decrypt(ciphs[i], ptxt);
    std::vector<uint64_t> decrypted;
    encoder.decode(ptxt, decrypted);
    for (uint64_t j = 0; j < (slots >> 1); j++) {
      uint64_t val = decrypted[j];
      if (cap_negative && val > (plain_mod >> 1)) {
        // only interested in interval positive part of result
        val = 0;
      }
      res.push_back(val);
      if (res.size() == k) break;
    }
  }
  return true;
}

//----------------------------------------------------------------

int Client::get_noise(seal::Ciphertext &ciph) {
  return decryptor.invariant_noise_budget(ciph);
}

//----------------------------------------------------------------
void Client::decrypt(std::vector<uint64_t> &res, Ciphertext &ciph) {
  Plaintext ptxt;
  decryptor.decrypt(ciph, ptxt);
  encoder.decode(ptxt, res);
}
