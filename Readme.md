# Aggregated PIR for creating a corona heatmap

This repository contains the source code of the paper [1,2] by researchers of TU Graz. A current version of the paper is also present in this repository. For more information on the project, visit https://covid-heatmap.iaik.tugraz.at/.

### Source code
The code is based on Microsoft SEAL v3.5 (https://github.com/Microsoft/SEAL) and is compatible with Windows and Linux.
This repository contains a demo application to perform the matrix multiplication on encrypted data and to compute the challenge masks. Use `params.h` for a parametrization of the code.

### Compilation:
Execute the following commands to compile the source code:
```
bash ./build_seal.sh
mkdir build
cd build
cmake ..
make
```

### Citing our work:
Please use the following BibTeX entry to cite our work in academic papers.

```tex
@article{APIR,
  author    = {Lukas Helminger and
              Daniel Kales and
              Christian Rechberger and
              Roman Walch},
  title     = {Aggregated Private Information Retrieval: {A} First Practical Implementation
              to Support Large-Scale Disease Analytics},
  journal   = {CoRR},
  volume    = {abs/2005.02061},
  year      = {2020}
}
```


[1] https://eprint.iacr.org/2020/522 <br/>
[2] https://arxiv.org/abs/2005.02061
