#include <iostream>

#include "params.h"
#include <fstream>

#include "Client.h"
#include "Server.h"

void getInput(std::string& path, std::vector<int64_t>& input){
  std::ifstream is;
  is.open(path);
  if(!is.is_open()){
    std::cout << "ERROR: Unable to open file: " << path << std::endl;
    exit(-1);
  }
  std::string in;
  int64_t intermediate;

  while(!is.eof()){
    std::getline(is, in, '\n');
    std::istringstream iss(in);
    iss >> intermediate;
    input.push_back(intermediate);
  }
}

void writeOutput(std::string& path, std::vector<int64_t>& output){
  std::ofstream os;
  os.open(path);
  if(!os.is_open()){
    std::cout << "ERROR: Unable to open file: " << path << std::endl;
    exit(-1);
  }
  for(uint64_t i = 0; i < output.size(); i++){
    if(i != 0){
      os << std::endl;
    }
    os << std::to_string(output[i]);
  }
  os.close();
}



int main(int argc, char *argv[]) {

  if(argc != 4){
    std::cout << "usage: " << argv[0] << " [in_file_name] [out_file_name] [privacy]" << std::endl;
  }

  std::string in_file = argv[1];
  std::string out_file = argv[2];
  std::vector<int64_t> in, out;
  double privacy = std::strtod(argv[3], nullptr);

  std::cout << "running file: " << in_file << " output: " << out_file << " with privacy: " << privacy << std::endl;

  getInput(in_file, in);

  auto context = Client::create_context(PPA_PARAM::MOD_DEGREE, PPA_PARAM::PLAIN_MODULUS, PPA_PARAM::sec80);
  Server server(context, PPA_PARAM::PLAIN_MODULUS, PPA_PARAM::USE_BSGS);
  server.activate_diff_priv(true, 1, privacy);

  for (int64_t i : in) {
    out.push_back(i + server.laplace_round());
  }

  writeOutput(out_file, out);

  return 0;
}
