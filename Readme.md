# Privately Connecting Mobility to Infectious Diseases via Applied Cryptography

This repository contains the source code of the paper [1,2] by researchers of TU Graz. A current version of the paper is also present in this repository. For more information on the project, visit https://covid-heatmap.iaik.tugraz.at/.

### Source code
The code is based on Microsoft SEAL (https://github.com/Microsoft/SEAL) and is compatible with Windows and Linux.
This repository contains a demo application to perform the matrix multiplication on encrypted data and to compute the challenge mask. Use `params.h` for a parametrization of the code.

### Compilation:
Execute the following commands to compile the source code:
```
git submodule update --init
cd SEAL
cmake .
make
cd ..
mkdir build
cd build
cmake ..
make
```

### Acknowledgements

![Safe-DEED Logo](https://github.com/IAIK/CoronaHeatMap/blob/master/Safe-DEED_logo.png)

This project has received funding from the European Union’s Horizon 2020 research and innovation programme under grant agreement No 825225.

https://safe-deed.eu/

### Citing our work:
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


[1] https://eprint.iacr.org/2020/522 <br/>
[2] https://arxiv.org/abs/2005.02061
