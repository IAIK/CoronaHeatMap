#!/usr/bin/env bash

git submodule update --init --recursive

cd SEAL \
  && cmake . \
  && make -j4 \
  && cd ..
