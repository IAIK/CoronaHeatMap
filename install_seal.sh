#!/usr/bin/env bash


# Install SEAL
git clone https://github.com/microsoft/SEAL.git \
  && cd SEAL/native/src \
  && git checkout 3.4.5 \
  && cmake . \
  && make -j4 \
  && cd ../../..
