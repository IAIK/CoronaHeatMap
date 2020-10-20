#include "Server.h"
#include <iostream>
#include "shake.h"
#include <fstream>
#include <assert.h>

using namespace seal;

//----------------------------------------------------------------

Server::Server(std::shared_ptr<seal::SEALContext> con, uint64_t p_mod, bool bsgs)
       : context(con), evaluator(context), encoder(context),
         plain_mod(p_mod), slots(encoder.slot_count()),
         use_bsgs(bsgs)
{
  set_bsgs_params();
  init_and_seed_shake();
}

//----------------------------------------------------------------

void Server::set_gk(seal::GaloisKeys& galois) {
  galois_keys = galois;
  gk_set = true;
}

//----------------------------------------------------------------

void Server::set_rk(seal::RelinKeys& relin) {
  relin_keys = relin;
  relin_set = true;
}

//----------------------------------------------------------------

void Server::set_num_threads(uint64_t num) {
  num_threads = num;
}

//----------------------------------------------------------------

void Server::diagonal(Ciphertext& state,
              const plain_matrix& mat) {
  size_t matrix_dims = encoder.slot_count() >> 1;

  // diagonal method preperation:
  std::vector<Plaintext> matrix;
  matrix.reserve(matrix_dims);
  for (auto i = 0ULL; i < matrix_dims; i++) {
    std::vector<uint64_t> diag;
    diag.reserve(2 * matrix_dims);
    for (auto j = 0ULL; j < matrix_dims; j++) {
      diag.push_back(mat[(i + j) % matrix_dims][j]);
    }
    // encode second matrix as well
    for (auto j = 0ULL; j < matrix_dims; j++) {
      diag.push_back(mat[(i + j) % matrix_dims + matrix_dims][j]);
    }
    Plaintext row;
    encoder.encode(diag, row);
    matrix.push_back(row);
  }

  Ciphertext sum = state;
  evaluator.multiply_plain_inplace(sum, matrix[0]);
  for (auto i = 1ULL; i < matrix_dims; i++) {
    Ciphertext tmp;
    evaluator.rotate_rows_inplace(state, 1, galois_keys);
    evaluator.multiply_plain(state, matrix[i], tmp);
    evaluator.add_inplace(sum, tmp);
  }
  state = sum;
}

//----------------------------------------------------------------

void Server::babystep_giantstep(Ciphertext& state,
                        const plain_matrix& mat) {
  size_t matrix_dims = encoder.slot_count() >> 1;

  // baby step giant step method preperation:
  std::vector<Plaintext> matrix;
  matrix.reserve(matrix_dims);
  for (auto i = 0ULL; i < matrix_dims; i++) {
    std::vector<uint64_t> diag;
    std::vector<uint64_t> tmp;
    diag.reserve(2 * matrix_dims);
    tmp.reserve(matrix_dims);
    for (auto j = 0ULL; j < matrix_dims; j++) {
      diag.push_back(mat[(i + j) % matrix_dims][j]);
      tmp.push_back(mat[(i + j) % matrix_dims + matrix_dims][j]);
    }
    // rotate:
    auto l = i / bsgs_n1;
    if (l) {
      std::rotate(diag.begin(), diag.begin() + diag.size() - l * bsgs_n1, diag.end());
      std::rotate(tmp.begin(), tmp.begin() + tmp.size() - l * bsgs_n1, tmp.end());
    }
    for (auto& el : tmp)
      diag.push_back(el);

    Plaintext row;
    encoder.encode(diag, row);
    matrix.push_back(row);
  }

  Ciphertext temp;
  Ciphertext outer_sum;
  Ciphertext inner_sum;
  // prepare rotations
  std::vector<Ciphertext> rot;
  rot.resize(bsgs_n1);
  rot[0] = state;
  for (uint64_t j = 1; j < bsgs_n1; j++)
    evaluator.rotate_rows(rot[j - 1], 1, galois_keys, rot[j]);

  for (uint64_t l = 0; l < bsgs_n2; l++) {
    evaluator.multiply_plain(rot[0], matrix[l * bsgs_n1], inner_sum);
    for (uint64_t j = 1; j < bsgs_n1; j++) {
      evaluator.multiply_plain(rot[j], matrix[l * bsgs_n1 + j], temp);
      evaluator.add_inplace(inner_sum, temp);
    }
    if (!l)
      outer_sum = inner_sum;
    else {
      auto tmp = l * bsgs_n1;
      evaluator.rotate_rows_inplace(inner_sum, tmp, galois_keys);
      evaluator.add_inplace(outer_sum, inner_sum);
    }
  }
  state = outer_sum;
}

//----------------------------------------------------------------

void Server::set_bsgs_params() {
  uint64_t deg = ceil(log2((double)(slots >> 1)));
  uint64_t deg1 = deg >> 1;
  if (deg1 * 2 != deg)
    deg1++;
  uint64_t deg2 = deg - deg1;
  bsgs_n1 = (1ULL << deg1);
  bsgs_n2 = (1ULL << deg2);
  assert((slots >> 1) == bsgs_n1 * bsgs_n2);
}

//----------------------------------------------------------------

void Server::set_dimension(uint64_t N_in, uint64_t k_in) {
  this->N = N_in;
  this->k = k_in;

  num_plaintexts = ceil((double)N / slots);
  num_ciphertexts = ceil(2 * (double)k / slots);
}

//----------------------------------------------------------------

bool Server::compute(std::vector<seal::Ciphertext>& out) {
  if (!gk_set || !in_set)
    return false;

  return compute_internal(out);
}

//----------------------------------------------------------------

bool Server::computeWithMask(std::vector<seal::Ciphertext>& out, uint64_t hw) {
  if (!gk_set || !relin_set || !in_set)
    return false;

  return compute_internal(out, hw, true);
}

//----------------------------------------------------------------

bool Server::compute_internal(std::vector<seal::Ciphertext>& out, uint64_t hw, bool masking) {

  out.clear();
  out.resize(num_ciphertexts);
  if (input.size() != num_plaintexts)
    return false;

  std::cout << std::endl;
  std::vector<Ciphertext> mask;

  if (num_threads == 1) {
    if (masking) {
      std::cout << "    Computing mask..." << std::flush;
      computeMask(mask, hw);
      std::cout << "...done" << std::endl;
    }
    runner(out, input, 0, num_plaintexts, *this);
    std::cout << "    Switch to last level..." << std::flush;
    for (uint64_t i = 0; i < num_ciphertexts; i++) {
      if (masking)
        evaluator.add_inplace(out[i], mask[i]);
      if (diff_priv) {
        std::vector<uint64_t> noise_plain(slots);
        for (uint64_t s = 0; s < slots; s++)
          noise_plain[i] = laplace_round();
        Plaintext noise;
        encoder.encode(noise_plain, noise);
        evaluator.add_plain_inplace(out[i], noise);
      }
      evaluator.mod_switch_to_inplace(out[i], context->last_parms_id());
    }
    std::cout << "...done" << std::endl;
    return true;
  }

  // MULTITHREAD

  uint64_t num = ceil((double)num_plaintexts / num_threads);

  // capping threads
  uint64_t n_threads = num_threads;
  for (uint64_t i = 0; i < n_threads; i++) {
    if (i * num >= num_plaintexts) {
      n_threads = i;
      break;
    }
  }

  std::cout << "    Spawning " << n_threads << " threads, each calculating " << num << " rows..." << std::flush;

  std::vector<std::thread> threads;
  std::vector<std::vector<Ciphertext>> t_res;
  threads.reserve(n_threads);
  t_res.resize(n_threads, std::vector<Ciphertext>(num_ciphertexts));
  for (uint64_t i = 0; i < n_threads; i++) {
    std::thread t(runner, std::ref(t_res[i]), std::ref(input), i * num, num, std::ref(*this));
    threads.push_back(std::move(t));
  }
  std::cout << "...done" << std::endl;

  if (masking) {
    std::cout << "    Computing mask..." << std::flush;
    computeMask(mask, hw);
    std::cout << "...done" << std::endl;
  }

  std::cout << "    Waiting..." << std::flush;
  for (auto& t : threads) {
  // If thread Object is Joinable then Join that thread.
    if (t.joinable())
      t.join();
  }
  std::cout << "...done" << std::endl;

  std::cout << "    Combining results from threads and switch to last level..." << std::flush;
  for (uint64_t i = 0; i < num_ciphertexts; i++) {
      out[i] = t_res[0][i];
    for (uint64_t t = 1; t < n_threads; t++) {
      evaluator.add_inplace(out[i], t_res[t][i]);
    }
    if (masking)
      evaluator.add_inplace(out[i], mask[i]);
    if (diff_priv) {
      std::vector<uint64_t> noise_plain(slots);
      for (uint64_t s = 0; s < slots; s++)
        noise_plain[i] = laplace_round();
      Plaintext noise;
      encoder.encode(noise_plain, noise);
      evaluator.add_plain_inplace(out[i], noise);
    }
    evaluator.mod_switch_to_inplace(out[i], context->last_parms_id());
  }
  std::cout << "...done" << std::endl;
  return true;
}

//----------------------------------------------------------------

void Server::sum_elements(seal::Ciphertext& in_out) {
  Ciphertext rotated;
  uint64_t rot_index = 1;
  while(rot_index < (slots >> 1)) {
    evaluator.rotate_rows(in_out, rot_index, galois_keys, rotated);
    evaluator.add_inplace(in_out, rotated);
    rot_index *= 2;
  }
  evaluator.rotate_columns(in_out, galois_keys, rotated);
  evaluator.add_inplace(in_out, rotated);
}

//----------------------------------------------------------------

// dot product with 1
void Server::dot1(const seal::Ciphertext& in,
          seal::Ciphertext& out) {

  out = in;
  sum_elements(out);
}

//----------------------------------------------------------------

// dot product
void Server::dot(const seal::Ciphertext& in1,
         seal::Ciphertext& in_out) {

  evaluator.multiply_inplace(in_out, in1);
  evaluator.relinearize_inplace(in_out, relin_keys);
  sum_elements(in_out);
}

//----------------------------------------------------------------

void Server::minus_1(const std::vector<seal::Ciphertext>& in,
             std::vector<seal::Ciphertext>& out) {

  out.reserve(in.size());

  Plaintext one;
  encoder.encode(std::vector<uint64_t>(slots, 1), one);

  for(auto& el : in) {
    Ciphertext tmp;
    evaluator.sub_plain(el, one, tmp);
    out.push_back(std::move(tmp));
  }
}

//---------------------------------------------------------------

void Server::computeHWMask(Ciphertext& mask,
                   const std::vector<seal::Ciphertext>& in,
                   uint64_t hw) {

  Plaintext W;
  encoder.encode(std::vector<uint64_t>(slots, hw), W);

  std::vector<Ciphertext> tmp(in.size());
  for(uint64_t i = 0; i < in.size(); i++) {
    dot1(in[i], tmp[i]);
  }
  evaluator.add_many(tmp, mask);

  evaluator.sub_plain_inplace(mask, W);
}

//----------------------------------------------------------------

void Server::computePartBinMask(Ciphertext& mask,
                   const std::vector<seal::Ciphertext>& in,
                   const std::vector<seal::Ciphertext>& in_minus_1) {

  uint64_t y = generate_random_field_element_without_0();
  uint64_t r = generate_random_field_element_without_0();


  // y vector
  std::vector<Plaintext> y_enc(in.size());
#ifdef __linux__
  uint64_t start_val = r;
  for (uint64_t j = 0; j < in.size(); j++) {
    std::vector<uint64_t> y_decode(slots, start_val);
    for (uint64_t i = 1; i < slots; i++) {
      uint64_t tmp = ((__uint128_t)y_decode[i - 1] * y) % plain_mod;
      y_decode[i] = tmp;
    }
    uint64_t tmp = ((__uint128_t)y_decode[slots - 1] * y) % plain_mod;
    start_val = tmp;
    encoder.encode(y_decode, y_enc[j]);
  }
#else
  uint64_t start_val = r;
  seal::BigUInt bigy(128, y);
  for (uint64_t i = 0; i < in.size(); i++) {
    std::vector<uint64_t> y_decode(slots, start_val);
    for (uint64_t j = 1; j < slots; j++) {
      seal::BigUInt bigtmp(128, y_decode[j - 1]);
      seal::BigUInt bigrem;
      bigtmp = bigtmp * bigy;
      bigtmp.divrem(plain_mod, bigrem);

      uint64_t tmp = bigrem.data()[0]; // this seems not that great
      y_decode[j] = tmp;
    }
    seal::BigUInt bigtmp(128, y_decode[slots - 1]);
    seal::BigUInt bigrem;
    bigtmp = bigtmp * bigy;
    bigtmp.divrem(plain_mod, bigrem);

    uint64_t tmp = bigrem.data()[0]; // this seems not that great
    start_val = tmp;
    encoder.encode(y_decode, y_enc[i]);
  }
#endif

  std::vector<Ciphertext> dy(in.size());

  // d*y
  for (uint64_t i = 0; i < in.size(); i++) {
    evaluator.multiply_plain(in_minus_1[i], y_enc[i], dy[i]);
  }

  for(uint64_t i = 0; i < in.size(); i++) {
    dot(in[i], dy[i]);
  }
  evaluator.add_many(dy, mask);

}

void Server::computeBinMask(Ciphertext& mask,
                   const std::vector<seal::Ciphertext>& in,
                   const std::vector<seal::Ciphertext>& in_minus_1) {
  Ciphertext mask1;

  computePartBinMask(mask, in, in_minus_1);
  computePartBinMask(mask1, in, in_minus_1);

  evaluator.add_inplace(mask, mask1);
}

//----------------------------------------------------------------

void Server::computeMask(std::vector<Ciphertext>& mask,
                uint64_t hw) {

  // d vector
  std::vector<Ciphertext> d;
  minus_1(input, d);

  // first operand
  Ciphertext mask_hw;
  computeHWMask(mask_hw, input, hw);
  uint64_t z = generate_random_field_element();
  Plaintext Z;
  encoder.encode(std::vector<uint64_t>(slots, z), Z);
  evaluator.multiply_plain_inplace(mask_hw, Z);

  // second operand (consisting of two randomized masks)
  Ciphertext mask_bin;
  computeBinMask(mask_bin, input, d);

  // final add
  evaluator.add_inplace(mask_hw, mask_bin);

  // r vec
  std::vector<Plaintext> r_enc(num_ciphertexts);
  for (uint64_t i = 0; i < num_ciphertexts; i++) {
    std::vector<uint64_t> r_decode;
    r_decode.reserve(slots);
    for (uint64_t j = 0; j < slots; j++) {
      r_decode.push_back(generate_random_field_element_without_0());
    }
    encoder.encode(r_decode, r_enc[i]);
  }

  // randomizing the scalar mask for each slot
  mask.resize(num_ciphertexts);
  for(uint64_t i = 0; i < num_ciphertexts; i++) {
    evaluator.multiply_plain(mask_hw, r_enc[i], mask[i]);
  }
}

//----------------------------------------------------------------

void Server::runner(std::vector<seal::Ciphertext>& out,
            const std::vector<seal::Ciphertext>& in,
            uint64_t start_index,
            uint64_t num, Server& server) {

  // essentially big v^T * M mult
  for (uint64_t i = 0; i < server.num_ciphertexts; i++) {
    for (uint64_t j = start_index; j < start_index + num && j < server.num_plaintexts; j++) {
      Ciphertext r = in[j];
      plain_matrix mat;
      server.get_submatrix(mat, j, i);
      if (server.use_bsgs)
        server.babystep_giantstep(r, mat);
      else
        server.diagonal(r, mat);

      // two results are in the two rows of the ciphertexts
      Ciphertext r_rot;
      server.evaluator.rotate_columns(r, server.galois_keys, r_rot);
      server.evaluator.add_inplace(r, r_rot);
      if (j == start_index)
        out[i] = r;
      else
        server.evaluator.add_inplace(out[i], r);
    }
  }
}

//----------------------------------------------------------------
void Server::get_matrix(const std::string &path) {
  std::cout << "Get matrix from " << path << std::endl;
  if(!input_matrix.empty()){
      std::cout << "Warning, matrix was already initialized" << std::endl;
  }

  std::ifstream is;
  is.open(path);
  if(!is.is_open()){
      std::cout << "ERROR: Unable to open file: " << path << std::endl;
      exit(-1);
  }
  std::vector<std::vector<uint32_t>> input_m;
  std::vector<uint32_t> row;
  std::string line, word, temp;
  uint64_t intermediate;

  while (!is.eof()) {
      row.clear();
      getline(is, line);
      std::stringstream s(line);
      while (getline(s, word, ',')) {
          intermediate = std::stoi(word);
          row.push_back(intermediate);
      }
      input_m.push_back(row);
  }
  input_matrix = input_m;
}

inline uint32_t Server::get_matrix_element(uint64_t row, uint64_t col) {
  uint32_t val = 0;
  if (row >= N || col >= k)
  {
    return 0;
  }

  if (random_matrix) {
    return col * col - col * row + row * row;
  }

  matrix_mutex.lock();

  if(input_matrix.empty()){
      get_matrix(matrix_path);
      if(input_matrix.empty() || input_matrix.size() <= N){
          std::cout << "cannot get element from empty matrix: " << "vienna_matrix.csv" << std::endl;
          matrix_mutex.unlock();
          exit(-1);
      }
  }
  if(input_matrix[row].size() <= col){
    std::cout << "illegal access at: " << row << ", " << col << std::endl;
    matrix_mutex.unlock();
    exit(-1);
  }
  val = input_matrix[row][col];
  matrix_mutex.unlock();
  return val;
}

//----------------------------------------------------------------

void Server::get_submatrix(plain_matrix& out, uint64_t row, uint64_t col) {
  size_t halfslots = (slots >> 1);
  out.reserve(slots);
  for (uint64_t i = 0; i < slots; i++) {
    std::vector<uint32_t> out_row;
    out_row.reserve(halfslots);
    for (uint64_t j = 0; j < halfslots; j++) {
      uint64_t row_index = row * slots + i;
      uint64_t col_index = col * halfslots + j;
      uint32_t mat_val = get_matrix_element(row_index, col_index);
      out_row.push_back(mat_val);
    }
    out.push_back(out_row);
  }
}

//----------------------------------------------------------------

void Server::set_input(std::vector<seal::Ciphertext>& in) {
  input = in;
  in_set = true;
}

//----------------------------------------------------------------

void Server::runner_plain(std::vector<uint64_t>& out,
                  const std::vector<uint64_t>& in,
                  uint64_t start_index,
                  uint64_t num, Server& server) {

  for (uint64_t col = 0; col < server.k; col++) {
    uint64_t sum = 0;
    for (uint64_t row = start_index; row < start_index + num && row < server.N; row++) {
      uint32_t mat_val = server.get_matrix_element(row, col);
      sum = (sum + (uint64_t)(mat_val) * in[row]) % server.plain_mod;
    }
    out[col] = sum;
  }
}


//----------------------------------------------------------------

void Server::compute_plain(std::vector<uint64_t>& out,
                  const std::vector<uint64_t>& in) {
  // essentially big v^T * M mult
  out.clear();
  out.resize(k);

  if (num_threads == 1) {
    runner_plain(out, in, 0, N, *this);
    if (diff_priv) {
      for (auto &el : out)
        el += laplace_round();
    }
    return;
  }

  // MULTITHREAD

  std::cout << std::endl;
  uint64_t num = ceil((double)N / num_threads);

  // capping threads
  uint64_t n_threads = num_threads;
  for (uint64_t i = 0; i < n_threads; i++) {
    if (i * num >= N) {
      n_threads = i;
      break;
    }
  }

  std::cout << "    Spawning " << n_threads << " threads, each calculating " << num << " rows..." << std::flush;

  std::vector<std::thread> threads;
  std::vector<std::vector<uint64_t>> t_res;
  threads.reserve(n_threads);
  t_res.resize(n_threads, std::vector<uint64_t>(k));
  for (uint64_t i = 0; i < n_threads; i++) {
    std::thread t(runner_plain, std::ref(t_res[i]), std::ref(in), i * num, num, std::ref(*this));
    threads.push_back(std::move(t));
  }
  std::cout << "...done" << std::endl;

  std::cout << "    Waiting..." << std::flush;
  for (auto& t : threads) {
  // If thread Object is Joinable then Join that thread.
    if (t.joinable())
      t.join();
  }
  std::cout << "...done" << std::endl;

  std::cout << "    Combining results from threads..." << std::flush;
  for (uint64_t i = 0; i < k; i++) {
      out[i] = t_res[0][i];
    for (uint64_t t = 1; t < n_threads; t++) {
      out[i] = (out[i] + t_res[t][i]) % plain_mod;
    }
    if (diff_priv)
      out[i] += laplace_round();
  }
  std::cout << "...done" << std::endl;
}

//----------------------------------------------------------------

bool Server::ciphers_from_file() {
  input.reserve(num_plaintexts);
  std::ifstream c;
  c.open(CIPHER_FILE);
  try{
    while(true) {
      Ciphertext ct;
      ct.load(context, c);
      input.push_back(ct);
    }
  }
  catch(std::exception& e)
  {}
  in_set = true;

  return (input.size() == num_plaintexts);
}

//----------------------------------------------------------------

std::shared_ptr<seal::SEALContext> Server::context_from_file(bool sec80) {
  EncryptionParameters parms;

  std::ifstream param;
  param.open(PARAM_FILE);
  parms.load(param);

  sec_level_type sec = sec_level_type::tc128;
  if (sec80) {
    sec = sec_level_type::none;
  }
  return SEALContext::Create(parms, true, sec);
}

//----------------------------------------------------------------

uint64_t Server::keys_from_file(bool masking) {

  std::ifstream gk;
  gk.open(GK_FILE);
  galois_keys.load(context, gk);
  gk_set = true;

  if (masking) {
    std::ifstream rk;
    rk.open(RK_FILE);
    relin_keys.load(context, rk);
    relin_set = true;
  }

  uint64_t hw;
  std::ifstream h;
  h.open(HW_FILE);
  h >> hw;
  std::cout << "...done" << std::endl;
  std::cout << "Hamming weight is " << hw << std::endl;
  return hw;
}

//----------------------------------------------------------------

void Server::print_parameters() {
  // Verify parameters
  if (!context) {
    throw std::invalid_argument("context is not set");
  }
  auto& context_data = *context->key_context_data();

  /*
  Which scheme are we using?
  */
  std::string scheme_name;
  switch (context_data.parms().scheme()) {
  case scheme_type::BFV:
    scheme_name = "BFV";
    break;
  case scheme_type::CKKS:
    scheme_name = "CKKS";
    break;
  default:
    throw std::invalid_argument("unsupported scheme");
  }
  std::cout << "/" << std::endl;
  std::cout << "| Encryption parameters:" << std::endl;
  std::cout << "|   scheme: " << scheme_name << std::endl;
  std::cout << "|   poly_modulus_degree: " << context_data.parms().poly_modulus_degree()
            << std::endl;

  /*
  Print the size of the true (product) coefficient modulus.
  */
  std::cout << "|   coeff_modulus size: ";
  std::cout << context_data.total_coeff_modulus_bit_count() << " (";
  auto coeff_modulus          = context_data.parms().coeff_modulus();
  std::size_t coeff_mod_count = coeff_modulus.size();
  for (std::size_t i = 0; i < coeff_mod_count - 1; i++) {
    std::cout << coeff_modulus[i].bit_count() << " + ";
  }
  std::cout << coeff_modulus.back().bit_count();
  std::cout << ") bits" << std::endl;

  /*
  For the BFV scheme print the plain_modulus parameter.
  */
  if (context_data.parms().scheme() == scheme_type::BFV) {
    std::cout << "|   plain_modulus: " << context_data.parms().plain_modulus().value() << std::endl;
  }

  std::cout << "\\" << std::endl;
}

//----------------------------------------------------------------

void Server::ciphers_to_file(std::vector<seal::Ciphertext>& ciphers) {
  std::ofstream c_out;
  c_out.open(RESULT_FILE);
  for (auto& ct : ciphers) {
    ct.save(c_out);
  }
}

//----------------------------------------------------------------
void Server::activate_diff_priv(bool activate, u_int64_t sens, double eps) {
  diff_priv = activate;
  sensitivity = sens;
  epsilon = eps;
}

//----------------------------------------------------------------
// Generates samples from the Laplace Distribution according to the
// Ratio of Uniforms method outlined in Section 4.7
// Devroye, Luc. "Non-Uniform Random Variate Generation" (1987): 195. Cleaner
// and more accurate than the typical Inverse CDF method under fixed precision
// arithmetic.
double Server::laplace() const {
  double u1 = UniformDouble();
  double u2 = UniformDouble();

  double scale = (double)sensitivity / epsilon;

  double value = std::log(u1 / u2) * (scale);
  if (std::isnan(value)) {
    return 0.0;
  }

  return value;
}

//----------------------------------------------------------------

int64_t Server::laplace_round() const {
  int64_t res = round(laplace());
  return res;
}

//----------------------------------------------------------------

void Server::set_random_matrix(bool val) {
  random_matrix = val;
}

void Server::set_matrix_path(const std::string& path) {
  matrix_path = path;
}
