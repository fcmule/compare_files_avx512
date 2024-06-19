A simple command line utility to check if two files are equal.
Requires a 64-bit processor with AVX-512 extensions support.

The utility has been benchmarked and compared with the equivalent "cmp" command using the "enwik9" data,
obtaining the following results (lower is better):

Command                            | Time (10 runs avg)
--------------------------------------------------------
cmp enwik9 enwik9                  | 3.722 s +/- 0.247 s
--------------------------------------------------------
compare_files_avx512 enwik9 enwik9 | 1.020 s +/- 0.022 s
--------------------------------------------------------

Usage:

./build.sh
./build/compare_files_avx512 [file1_path] [file2_path]
