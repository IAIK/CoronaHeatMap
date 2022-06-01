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
  client.create_gk(true, PPA_PARAM::USE_BSGS);
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
  auto rlk = client.get_relin_keys();
  server.set_rk(rlk);
  server.set_num_threads(PPA_PARAM::NUM_THREADS);
  server.set_dimension(PPA_PARAM::N, PPA_PARAM::k);
  server.set_input(ciphs);

  //----------------------------------------------------------------

  std::chrono::high_resolution_clock::time_point time_start, time_end;
  std::chrono::milliseconds time_diff;

  std::cout << "Computing mask..." << std::flush;
  std::vector<Ciphertext> mask;
  time_start = std::chrono::high_resolution_clock::now();
  server.computeMask(mask);
  time_end = std::chrono::high_resolution_clock::now();
  time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(time_end -
                                                                    time_start);
  std::cout << "...done" << std::endl;
  std::cout << "Time: " << time_diff.count() << " milliseconds" << std::endl;

  //----------------------------------------------------------------

  std::cout << "Final noise:" << std::endl;
  client.print_noise(mask);
  std::cout << "Decrypting mask..." << std::flush;

  std::vector<uint64_t> res;
  client.decrypt(res, mask);
  std::cout << "...done" << std::endl;

  std::cout << "Result:" << std::endl;
  bool correct = true;

  for (auto el : res) {
    if (el != 0) correct = false;
  }

  if (correct)
    std::cout << "Test passed!" << std::endl;
  else
    std::cout << "Test failed..." << std::endl;

  return 0;
}
