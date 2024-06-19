#!/bin/sh

set -e
mkdir -p build

clang src/compare_files_avx512.c -Wall -Wpedantic -O3 -mavx512bw -o build/compare_files_avx512
