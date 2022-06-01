# Privately Connecting Mobility to Infectious Diseases via Applied Cryptography

This repository contains the source code of the paper `Privately Connecting Mobility to Infectious Diseases via Applied Cryptography`. [1, 2]

## Source code

The code is based on Microsoft [SEAL](https://github.com/Microsoft/SEAL) (version 3.6.6) and is compatible with Windows and Linux.
This repository contains a demo application (using random data) to perform the matrix multiplication on encrypted data, including the proving mask, noise flooding, and differential privacy. Use `params.h` for a parametrization of the code.

## Compilation

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
| main       |           | Executes the client and the server                                               |
| client     | enc       | Creates keys, encrypts an input string, and stores everything on the file system |
| server     |           | Takes ciphertexts and keys from the file system and executes the protocol        |
| client     | dec       | Takes ciphertexts from the file system and decrypts the result of the protocol   |

## Docker

One can use docker to compile and run the code. The following command creates a docker container with the compiled executables being present in the `app` folder:

```bash
docker build -t heatmap .
```

Use the following command to get a bash shell inside the `app` folder of the container:

```bash
docker run -it heatmap
```

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
