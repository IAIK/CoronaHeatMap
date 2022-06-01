#!/usr/bin/env bash

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
