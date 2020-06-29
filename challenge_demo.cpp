#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "params.h"
#include "Server.h"
#include "Client.h"

using namespace seal;

int main() {
  auto context = Client::create_context(PPA_PARAM::MOD_DEGREE, PPA_PARAM::PLAIN_MODULUS, PPA_PARAM::sec80);

  std::cout << "Generating keys..." << std::flush;
  Client client(context, PPA_PARAM::PLAIN_MODULUS);
  client.print_parameters();
  std::cout << std::endl;
  client.set_dimension(PPA_PARAM::N, PPA_PARAM::k);
  client.create_gk(PPA_PARAM::NUM_MASKS, PPA_PARAM::MASKING, PPA_PARAM::USE_BSGS);
  std::cout << "...done" << std::endl;

  //----------------------------------------------------------------

  std::cout << "Reading " << PPA_PARAM::N << " inputs..." << std::flush;
  std::vector<uint64_t> input;
  client.get_input(input);
  std::cout << "...done" << std:: endl;

  //----------------------------------------------------------------

  std::cout << "Encrypting " << client.get_num_plaintexts() << " plaintexts with " << client.get_slots() << " slots each..." << std::flush;
  std::vector<Ciphertext> ciphs;
  uint64_t hw = client.encrypt(ciphs, input);
  std::cout << "...done" << std::endl;
  std::cout << "Hamming weight is " << hw << std::endl;

  if (hw < PPA_PARAM::MIN_HW) {
    std::cout << "Hamming weight is too small! Abort" << std::endl;
    exit(-1);
  }

  std::cout << "Initial noise:" << std::endl;
  client.print_noise(ciphs);

  //----------------------------------------------------------------

  Server server(context, PPA_PARAM::PLAIN_MODULUS, PPA_PARAM::USE_BSGS);
  server.set_gk(client.get_galois_keys());
  if (PPA_PARAM::MASKING || PPA_PARAM::NUM_MASKS > 0) {
    auto rlk = client.get_relin_keys();
    server.set_rk(rlk);
  }
  server.set_num_threads(PPA_PARAM::NUM_THREADS);
  server.set_dimension(PPA_PARAM::N, PPA_PARAM::k);
  server.set_input(ciphs);

  //----------------------------------------------------------------

  std::chrono::high_resolution_clock::time_point time_start, time_end;
  std::chrono::milliseconds time_diff;

  std::cout << "Computing challenge..." << std::flush;
  Ciphertext result_ciph;
  time_start = std::chrono::high_resolution_clock::now();
  server.createChallenge(result_ciph, ciphs, hw, PPA_PARAM::NUM_MASKS);
  time_end = std::chrono::high_resolution_clock::now();
  time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
  std::cout << "...done" << std::endl;
  std::cout << "Time: " << time_diff.count() << " milliseconds" << std::endl;

  //----------------------------------------------------------------

  std::cout << "Final noise:" << std::endl;
  int noise = client.get_noise(result_ciph);
  std::cout << "noise budget: " << noise << std::endl;  std::cout << "Decrypting mask..." << std::flush;

  std::vector<uint64_t> res;
  client.decrypt(res, result_ciph);
  std::cout << "...done" << std::endl;

  std::cout << "Result:" << std::endl;
  bool correct = server.correct_challenge(res);

  if (correct)
    std::cout << "Test passed!" << std::endl;
  else
    std::cout << "Test failed..." << std::endl;

  return 0;
}
