#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "params.h"
#include "src/Client.h"
#include "src/Server.h"

using namespace seal;

int main() {
  std::vector<std::pair<uint64_t, uint64_t>> dims;
  dims.push_back(std::make_pair(1, 1));
  dims.push_back(std::make_pair(5, 1));
  dims.push_back(std::make_pair(1, 5));
  dims.push_back(std::make_pair(1, 10));
  dims.push_back(std::make_pair(10, 1));
  dims.push_back(std::make_pair(1, 20));
  dims.push_back(std::make_pair(20, 1));
  dims.push_back(std::make_pair(5, 5));
  dims.push_back(std::make_pair(10, 5));

  //   constexpr bool masking = PPA_PARAM::MASKING;
  //   constexpr bool noise_flooding = PPA_PARAM::NOISE_FLOODING;
  //   constexpr bool mod_switch = PPA_PARAM::MOD_SWITCH;
  //   constexpr bool diff_priv = PPA_PARAM::DIFF_PRIV;
  //   constexpr uint64_t num_threads = PPA_PARAM::NUM_THREADS;
  constexpr bool masking = false;
  constexpr bool noise_flooding = false;
  constexpr bool mod_switch = false;
  constexpr bool diff_priv = false;
  constexpr uint64_t num_threads = 1;

  for (auto it : dims) {
    uint64_t N = it.first * PPA_PARAM::MOD_DEGREE;
    uint64_t k = it.second * PPA_PARAM::MOD_DEGREE / 2;

    uint64_t num_plaintexts = ceil((double)N / PPA_PARAM::MOD_DEGREE);
    uint64_t num_ciphertexts = ceil(2 * (double)k / PPA_PARAM::MOD_DEGREE);

    uint64_t matmuls = num_ciphertexts * num_plaintexts;

    std::cout << "---------------------------------------------------------\n";
    std::cout << "N = " << N << "\n";
    std::cout << "k = " << k << "\n";
    std::cout << "matmuls: " << matmuls << "\n";

    auto context = Client::create_context(
        PPA_PARAM::MOD_DEGREE, PPA_PARAM::PLAIN_MODULUS, PPA_PARAM::sec80);

    Client client(context, PPA_PARAM::PLAIN_MODULUS);
    client.print_parameters();
    std::cout << std::endl;
    client.set_dimension(N, k);
    client.create_gk(masking, PPA_PARAM::USE_BSGS);

    //---------------------------------------------------------------
    std::vector<uint64_t> input;
    client.get_input(input);

    //----------------------------------------------------------------

    std::vector<Ciphertext> ciphs;
    client.encrypt(ciphs, input);

    std::cout << "Initial noise:" << std::endl;
    client.print_noise(ciphs);

    //----------------------------------------------------------------

    Server server(context, PPA_PARAM::PLAIN_MODULUS, PPA_PARAM::USE_BSGS);
    server.set_gk(client.get_galois_keys());
    if (masking) {
      auto rlk = client.get_relin_keys();
      server.set_rk(rlk);
    }
    if (noise_flooding) {
      server.set_pk(client.get_public_key());
    }
    server.set_num_threads(num_threads);
    server.set_dimension(N, k);
    server.activate_mod_switch(mod_switch, PPA_PARAM::LEVELS_FROM_LAST);
    server.activate_diff_priv(diff_priv, PPA_PARAM::SENSITIVITY,
                              PPA_PARAM::EPSILON);
    server.activate_noise_flooding(noise_flooding,
                                   PPA_PARAM::NOISE_FLOODING_BITS);
    server.set_input(ciphs);

    std::chrono::high_resolution_clock::time_point time_start, time_end;
    std::chrono::milliseconds time_diff;

    std::cout << "Computing result..." << std::flush;
    std::vector<Ciphertext> result_ciph;
    time_start = std::chrono::high_resolution_clock::now();
    bool ok;
    if (masking)
      ok = server.computeWithMask(result_ciph);
    else
      ok = server.compute(result_ciph);

    time_end = std::chrono::high_resolution_clock::now();
    time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(
        time_end - time_start);
    std::cout << "...done" << std::endl;
    std::cout << "Time: " << time_diff.count() << " milliseconds" << std::endl;
    if (!ok) {
      std::cout << "Error occured... Abort" << std::endl;
      exit(-1);
    }

    std::vector<uint64_t> result_plain;
    time_start = std::chrono::high_resolution_clock::now();
    server.compute_plain(result_plain, input);
    time_end = std::chrono::high_resolution_clock::now();
    time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(
        time_end - time_start);

    //----------------------------------------------------------------

    std::cout << "Final noise:" << std::endl;
    client.print_noise(result_ciph);

    std::vector<uint64_t> res;
    ok = client.decrypt(res, result_ciph);

    std::cout << "Result:" << std::endl;
    if (res.size() != result_plain.size() || !ok)
      std::cout << "sizes differ, should be: " << result_plain.size() << ", is "
                << res.size() << std::endl;

    bool correct = true;
    for (uint64_t i = 0; i < k; i++) {
      if (res[i] != result_plain[i]) {
        correct = false;
        std::cout << "Error: First mismatch at index " << i << std::endl;
        break;
      }
    }

    if (correct)
      std::cout << "Test passed!" << std::endl;
    else
      std::cout << "Test failed..." << std::endl;
  }

  return 0;
}
