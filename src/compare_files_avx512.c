#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <immintrin.h>

static uint64_t get_file_size(FILE *file) {
    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: compare_files_avx512 [file1_path] [file2_path]\n");
        return 1;
    }

    char *file1_path = argv[1];
    char *file2_path = argv[2];

    FILE *file1 = fopen(file1_path, "rb");
    if (file1 == NULL) {
        fprintf(stderr, "Could not open file at path '%s'\n", file1_path);
        return 1;
    }

    FILE *file2 = fopen(file2_path, "rb");
    if (file2 == NULL) {
        fprintf(stderr, "Could not open file at path '%s'\n", file2_path);
        return 1;
    }

    uint64_t file1_size = get_file_size(file1);
    uint64_t file2_size = get_file_size(file2);

    if (file1_size != file2_size) {
        printf("The two files aren't equal, they have different sizes\n");
    } else {
        #define BUFFER_SIZE 4096
        #define BYTES_PER_LANE 64

        uint8_t buffer1[BUFFER_SIZE];
        uint8_t buffer2[BUFFER_SIZE];

        while (1) {
            // Since the two files have the same size, the amount of bytes
            // read from both is the same
            uint64_t read = fread(buffer1, 1, BUFFER_SIZE, file1);
            fread(buffer2, 1, BUFFER_SIZE, file2);

            uint64_t count = read / BYTES_PER_LANE;
            uint64_t left = read % BYTES_PER_LANE;

            uint8_t *b1 = buffer1;
            uint8_t *b2 = buffer2;

            while (count--) {
                __m512 b1_vec = _mm512_loadu_epi8(b1);
                __m512 b2_vec = _mm512_loadu_epi8(b2);

                __mmask64 b1_eq_b2 = _mm512_cmpeq_epi8_mask(b1_vec, b2_vec);

                if (b1_eq_b2 != 0xFFFFFFFFFFFFFFFF) {
                    printf("The two files aren't equal\n");
                    goto close_files_and_exit;
                }

                b1 += BYTES_PER_LANE;
                b2 += BYTES_PER_LANE;
            }

            while (left--) {
                if (*b1 != *b2) {
                    printf("The two files aren't equal\n");
                    goto close_files_and_exit;
                }

                ++b1;
                ++b2;
            }

            if (read < BUFFER_SIZE) {
                // We've read the entire files
                break;
            }
        }

        // If we're here no diffs were found
        printf("The two files are the same\n");
    }

close_files_and_exit:

    fclose(file1);
    fclose(file2);

    return 0;
}
