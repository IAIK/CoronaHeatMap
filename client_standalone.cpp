#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <assert.h>

#include "params.h"
#include "Client.h"

#ifdef CMP_CLIENT_PLAIN
  #include "Server.h" // just for plain computation
#endif

using namespace seal;


void enc() {

  auto context = Client::create_context(PPA_PARAM::MOD_DEGREE, PPA_PARAM::PLAIN_MODULUS, PPA_PARAM::sec80, true);

  std::cout << "Generating keys..." << std::flush;
  Client client(context, PPA_PARAM::PLAIN_MODULUS, true);
  client.print_parameters();
  std::cout << std::endl;
  client.set_dimension(PPA_PARAM::N, PPA_PARAM::k);
  client.create_gk(PPA_PARAM::MASKING, PPA_PARAM::USE_BSGS, true);
  std::cout << "...done" << std::endl;

  //----------------------------------------------------------------

  std::cout << "Reading " << PPA_PARAM::N << " inputs..." << std::flush;
  std::vector<uint64_t> input;
  client.get_input(input);
  std::cout << "...done" << std:: endl;

  //----------------------------------------------------------------

  std::cout << "Encrypting " << client.get_num_plaintexts() << " plaintexts with " << client.get_slots() << " slots each and write to file..." << std::flush;

  uint64_t hw = client.encrypt_to_file(input);

  std::cout << "...done" << std::endl;
  std::cout << "Hamming weight is " << hw << std::endl;
}

void dec() {
  //----------------------------------------------------------------
  std::cout << "Reading Keys..." << std::flush;
  auto context = Client::context_from_file(PPA_PARAM::sec80);

  SecretKey secret_key;
  Client::sk_from_file(secret_key, context);

  Client client(context, PPA_PARAM::PLAIN_MODULUS, secret_key);
  client.print_parameters();
  std::cout << std::endl;
  client.set_dimension(PPA_PARAM::N, PPA_PARAM::k);


  std::cout << "Reading Ciphertext..." << std::flush;
  std::vector<Ciphertext> ciphs;

  if (!client.ciphers_from_file(ciphs)) {
    std::cout << "Error occured... Abort" << std::endl;
    exit(-1);
  }

  std::cout << "Final noise:" << std::endl;
  client.print_noise(ciphs);
  std::cout << "Decrypting " << ciphs.size() << " ciphertexts..." << std::flush;


  std::vector<uint64_t> res;
  bool ok = client.decrypt(res, ciphs);
  std::cout << "...done" << std::endl;
  std::cout << "writing " << res.size() << " entries to file" << std::endl;
  Client::write_result_to_file(res, Client::RESULT_PLAIN);

  (void)ok;

  // ----------------------------------------------------------------
#ifdef CMP_CLIENT_PLAIN
  std::chrono::high_resolution_clock::time_point time_start, time_end;
  std::chrono::milliseconds time_diff;

  std::cout << "Compute result in plain to compare..." << std::flush;
  std::vector<uint64_t> input;
  client.get_input(input);


  Server server(context, PPA_PARAM::PLAIN_MODULUS, PPA_PARAM::USE_BSGS);
  server.set_num_threads(PPA_PARAM::NUM_THREADS);
  server.set_dimension(PPA_PARAM::N, PPA_PARAM::k);
  server.set_random_matrix(PPA_PARAM::RANDOM_MATRIX);

  std::vector<uint64_t> result_plain;
  time_start = std::chrono::high_resolution_clock::now();
  server.compute_plain(result_plain, input);
  time_end = std::chrono::high_resolution_clock::now();
  time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
  std::cout << "...done" << std::endl;
  std::cout << "Time: " << time_diff.count() << " milliseconds" << std::endl;


 //----------------------------------------------------------------

  std::cout << "Result:" << std::endl;
  if (res.size() != result_plain.size())
    std::cout << "sizes differ, should be: " << result_plain.size() << ", is " << res.size() << std::endl;

  bool correct = true;
  for (uint64_t i = 0; i < PPA_PARAM::k; i++) {
    if (res[i] != result_plain[i]) {
      correct = false;
      std::cout << "Error: First mismatch at index " << i << std::endl;
      break;
    }
  }

  if (correct) {
      std::cout << "Test passed!" << std::endl;
  }
  else
    std::cout << "Test failed..." << std::endl;

  std::cout << "...done" << std::endl;
#endif
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    std::cout << "usage: " << argv[0] << " [enc/dec]" << std::endl;
    return 0;
  }
  if (strcmp(argv[1], "enc") == 0)
    enc();
  else if (strcmp(argv[1], "dec") == 0)
    dec();
  else
    std::cout << "usage: " << argv[0] << " [enc/dec]" << std::endl;
  return 0;
}
