# Privately Connecting Mobility to Infectious Diseases via Applied Cryptography

This repository contains the source code of the paper `Privately Connecting Mobility to Infectious Diseases via Applied Cryptography`. [1, 2]

## Source code

The code is based on Microsoft [SEAL](https://github.com/Microsoft/SEAL) (version 3.6.6) and is compatible with Windows and Linux.
This repository contains a demo application (using random data) to perform the matrix multiplication on encrypted data, including the proving mask, noise flooding, and differential privacy. Use `params.h` for a parametrization of the code.

## Compilation

### Requirements

The source code just requires basic building tools (git, make, cmake) to compile. Use the following command to install these packages on Ubuntu:

```bash
sudo apt install git build-essential cmake
```

### Compiling Instructions

Execute the `compile.sh` script (which contains the following commands) to compile the source code:

```bash
git submodule update --init --recursive
cd SEAL
mkdir build
cd build
cmake ..
make -j 6
cd ../..
mkdir build
cd build
cmake ..
make -j 6
```

The binaries can then be found in the `bin` folder:

| Executable | Parameter | Description                                                                      |
|------------|-----------|----------------------------------------------------------------------------------|
| main       |           | Executes the whole protocol including the client and the server                                               |
| client     | enc       | Creates keys, encrypts an input string, and stores everything on the file system |
| server     |           | Takes ciphertexts and keys from the file system and executes the protocol        |
| client     | dec       | Takes ciphertexts from the file system and decrypts the result of the protocol   |

## Docker

One can use docker to compile and run the code. The following command creates a docker container with the compiled executables being present in the `app` folder:

```bash
docker build -t heatmap .
```

Use the following command to get a bash shell inside the `app` folder of the container, which contains all executables:

```bash
docker run -it heatmap
```

Binaries can then directly be executed from the current folder.

## Benchmarks in the Paper

The benchmarks in the paper create a heatmap for 8 million subscribers and were thus executed on Amazon AWS using a EC2 instance with 96 vCPU's @ 3.6 GHz. To reproduce these benchmarks, modify the values in `params.h` to:

```cpp
// matrix dimensions
constexpr uint64_t N = (1ULL << 23); // phone numbers
constexpr uint64_t k = (1ULL << 15); // cell sites

// Multithreading
constexpr uint64_t NUM_THREADS = 96;
```

Additionally, modify the following variables, if you want to use the 60-bit plaintext modulus instead of a 42-bit one:

```cpp
// BFV parameters
constexpr uint64_t PLAIN_MODULUS = 1103311814658949121ULL;  // 60 bit
constexpr uint64_t MAX_PRIME_SIZE = ((1ULL << 60) - 1);

// Server Mod_Switch
constexpr uint64_t LEVELS_FROM_LAST = 0;

// circuit privacy
constexpr uint64_t NOISE_FLOODING_BITS = 368;
```

Then, after compilation, to run the client and the server either run (in the `bin` folder, or directly in the docker):

```bash
./main # runs the client and the server and outputs the runtimes and noise budgets
```

or

```bash
./client enc # encrypts the input from the client and stores the results on the file system
./server # gets the client inputs from the file system and executes the protocol. Stores the result on the file system
./client dec # gets the server output from the file system and decrypts the result.
```

Both options perform the benchmarks, output the runtimes and noise budgets and should output "Test passed!" if the protocol produces a correct result.

### Acknowledgements

![Safe-DEED Logo](https://github.com/IAIK/CoronaHeatMap/blob/master/Safe-DEED_logo.png)

This project has received funding from the European Union’s Horizon 2020 research and innovation programme under grant agreement No 825225.

[https://safe-deed.eu/](https://safe-deed.eu/)

### Citing our work

Please use the following BibTeX entry to cite our work in academic papers.

```tex
@article{APIR,
  author    = {Alexandros Bampoulidis and
              Alessandro Bruni and
              Lukas Helminger and
              Daniel Kales and
              Christian Rechberger and
              Roman Walch},
  title     = {Privately Connecting Mobility to Infectious Diseases via Applied Cryptography},
  journal   = {CoRR},
  volume    = {abs/2005.02061},
  year      = {2020}
}
```

[1] [https://eprint.iacr.org/2020/522](https://eprint.iacr.org/2020/522)

[2] [https://arxiv.org/abs/2005.02061](https://arxiv.org/abs/2005.02061)
