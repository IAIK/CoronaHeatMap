#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "params.h"
#include "src/Client.h"
#include "src/Server.h"

using namespace seal;

int main() {
  auto context = Client::create_context(
      PPA_PARAM::MOD_DEGREE, PPA_PARAM::PLAIN_MODULUS, PPA_PARAM::sec80);

  std::cout << "Generating keys..." << std::flush;
  Client client(context, PPA_PARAM::PLAIN_MODULUS);
  client.print_parameters();
  std::cout << std::endl;
  client.set_dimension(PPA_PARAM::N, PPA_PARAM::k);
  client.create_gk(PPA_PARAM::MASKING, PPA_PARAM::USE_BSGS);
  std::cout << "...done" << std::endl;

  //----------------------------------------------------------------

  std::cout << "Reading " << PPA_PARAM::N << " inputs..." << std::flush;
  std::vector<uint64_t> input;
  client.get_input(input);
  std::cout << "...done" << std::endl;

  //----------------------------------------------------------------

  std::cout << "Encrypting " << client.get_num_plaintexts()
            << " plaintexts with " << client.get_slots() << " slots each..."
            << std::flush;
  std::vector<Ciphertext> ciphs;
  client.encrypt(ciphs, input);
  std::cout << "...done" << std::endl;

  std::cout << "Initial noise:" << std::endl;
  client.print_noise(ciphs);

  //----------------------------------------------------------------

  Server server(context, PPA_PARAM::PLAIN_MODULUS, PPA_PARAM::USE_BSGS);
  server.set_gk(client.get_galois_keys());
  if (PPA_PARAM::MASKING) {
    auto rlk = client.get_relin_keys();
    server.set_rk(rlk);
  }
  if (PPA_PARAM::NOISE_FLOODING) {
    server.set_pk(client.get_public_key());
  }
  server.set_num_threads(PPA_PARAM::NUM_THREADS);
  server.set_dimension(PPA_PARAM::N, PPA_PARAM::k);
  server.activate_mod_switch(PPA_PARAM::MOD_SWITCH,
                             PPA_PARAM::LEVELS_FROM_LAST);
  server.activate_diff_priv(PPA_PARAM::DIFF_PRIV, PPA_PARAM::SENSITIVITY,
                            PPA_PARAM::EPSILON);
  server.activate_noise_flooding(PPA_PARAM::NOISE_FLOODING,
                                 PPA_PARAM::NOISE_FLOODING_BITS);
  server.set_input(ciphs);

  std::chrono::high_resolution_clock::time_point time_start, time_end;
  std::chrono::milliseconds time_diff;

  std::cout << "Computing result..." << std::flush;
  std::vector<Ciphertext> result_ciph;
  time_start = std::chrono::high_resolution_clock::now();
  bool ok;
  if (PPA_PARAM::MASKING)
    ok = server.computeWithMask(result_ciph);
  else
    ok = server.compute(result_ciph);

  time_end = std::chrono::high_resolution_clock::now();
  time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(time_end -
                                                                    time_start);
  std::cout << "...done" << std::endl;
  std::cout << "Time: " << time_diff.count() << " milliseconds" << std::endl;
  if (!ok) {
    std::cout << "Error occured... Abort" << std::endl;
    exit(-1);
  }

  std::cout << "Compute result in plain to compare..." << std::flush;
  std::vector<uint64_t> result_plain;
  time_start = std::chrono::high_resolution_clock::now();
  server.compute_plain(result_plain, input);
  time_end = std::chrono::high_resolution_clock::now();
  time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(time_end -
                                                                    time_start);
  std::cout << "...done" << std::endl;
  std::cout << "Time: " << time_diff.count() << " milliseconds" << std::endl;

  //----------------------------------------------------------------

  std::cout << "Final noise:" << std::endl;
  client.print_noise(result_ciph);
  std::cout << "Decrypting " << client.get_num_ciphertexts()
            << " ciphertexts..." << std::flush;

  std::vector<uint64_t> res;
  ok = client.decrypt(res, result_ciph);
  std::cout << "...done" << std::endl;

  std::cout << "Result:" << std::endl;
  if (res.size() != result_plain.size() || !ok)
    std::cout << "sizes differ, should be: " << result_plain.size() << ", is "
              << res.size() << std::endl;

  bool correct = true;
  for (uint64_t i = 0; i < PPA_PARAM::k; i++) {
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

  return 0;
}
