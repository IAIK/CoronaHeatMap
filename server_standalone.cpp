#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

#include "params.h"
#include "Server.h"

using namespace seal;

int main() {
  std::cout << "Reading Keys..." << std::flush;
  auto context = Server::context_from_file(PPA_PARAM::sec80);

  Server server(context, PPA_PARAM::PLAIN_MODULUS, PPA_PARAM::USE_BSGS);
  server.set_num_threads(PPA_PARAM::NUM_THREADS);
  server.set_dimension(PPA_PARAM::N, PPA_PARAM::k);
  server.activate_diff_priv(PPA_PARAM::DIFF_PRIV, PPA_PARAM::SENSITIVITY, PPA_PARAM::EPSILON);
  server.set_random_matrix(PPA_PARAM::RANDOM_MATRIX);
  uint64_t hw = server.keys_from_file(PPA_PARAM::MASKING);

  if (hw < PPA_PARAM::MIN_HW) {
    std::cout << "Hamming weight is too small! Abort" << std::endl;
    exit(-1);
  }

  server.print_parameters();

  std::cout << "Reading Ciphertext..." << std::flush;
  server.ciphers_from_file();
  std::cout << "...done" << std::endl;

  //----------------------------------------------------------------

  std::chrono::high_resolution_clock::time_point time_start, time_end;
  std::chrono::milliseconds time_diff;

  std::cout << "Computing result..." << std::flush;
  std::vector<Ciphertext> result_ciph;
  time_start = std::chrono::high_resolution_clock::now();
  bool ok;
  if (PPA_PARAM::MASKING)
    ok = server.computeWithMask(result_ciph, hw);
  else
    ok = server.compute(result_ciph);
  time_end = std::chrono::high_resolution_clock::now();
  time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
  std::cout << "...done" << std::endl;
  std::cout << "Time: " << time_diff.count() << " milliseconds" << std::endl;
  if (!ok) {
    std::cout << "Error occured... Abort" << std::endl;
    exit(-1);
  }

  //----------------------------------------------------------------
  std::cout << "Writing to files..." << std::flush;
  server.ciphers_to_file(result_ciph);
  std::cout << "...done" << std::endl;

  return 0;
}
