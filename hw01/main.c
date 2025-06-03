#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int parse_word_encode(int* length)
{
    int ch = 0x0;
    int word = 0x0;
    int count = 0;

    while ((count < 4) && ((ch = getchar()) != EOF)) {
        word |= ch;

        if (count < 3) {
            word <<= 8;
        }
        count++;
    }

    *length = count;
    if (count == 0) {
        return word;
    }

    if (count < 4) {
        word <<= (3 - count) * 8;
    }

    return word;
}

bool is_parity_bit(int shift) {
    // Positions of parity bits relative to the lowest bit
    const int THIRD_PARITY = 35;
    const int FOURTH_PARITY = 31;
    const int FIFTH_PARITY = 23;
    const int SIXTH_PARITY = 7;

    // We do not need to check first or second parity bit, as they will be 0 by default
    return(shift == THIRD_PARITY || shift == FOURTH_PARITY || shift == FIFTH_PARITY || shift == SIXTH_PARITY);
}

long build_code_word(int word)
{
    long coded = 0x0L;
    int code_shift = 36;
    int word_pos = 31;

    while (word_pos >= 0) {
        if (is_parity_bit(code_shift)) {
            code_shift--;
            continue;
        }

        if (word & (1 << word_pos)) {
            coded |= 1L << code_shift;
        }
        code_shift--;
        word_pos--;
    }

    return coded;
}

long build_mask(int mask_pos)
{
    int pos = 0x0;
    long mask = 0x0L;

    while (pos < 5 * 8) {
        if (pos & (1L << mask_pos)) {
            mask |= 1L << (39 - pos);
        }
        pos++;
    }

    return mask;
}

int check_parity_bit(long mask, long word)
{
    int result = 0x0;
    long masked_word = mask & word;

    for (int pos = 39; pos >= 0; pos--) {
        if (masked_word & (1L << pos)) {
            result ^= 1;
        }
    }

    return result;
} 

long set_parity_bits(long word)
{
    long coded = word;
    int shift = 0x1;

    for (int parity_bit_number = 0; parity_bit_number <= 5; parity_bit_number++) {
        int word_pos = 39 - shift;
        long mask = build_mask(parity_bit_number);

        if (check_parity_bit(mask, word)) {
            coded |= 1L << word_pos;
        }
        shift <<= 1;
    }

    return coded;
}

void output_code(long code)
{
    int shift = 32;
    long mask = 0xFF00000000;

    while (shift >= 0) {
        char curr_byte = (code & mask) >> shift;
        putchar(curr_byte);
        shift -= 8;
        mask >>= 8;
    }
}

bool encode(void)
{
    int length = 0;

    while (!feof(stdin)){
        int word = parse_word_encode(&length);

        if (length == 0) {
            break;
        }

        long coded_without_parity = build_code_word(word);
        long code = set_parity_bits(coded_without_parity);
        output_code(code);
    }

    return true;
}

long parse_word_decode(int* length)
{
    int ch = 0x0;
    long code = 0x0;
    int count = 0;

    while ((count < 5) && ((ch = getchar()) != EOF)) {
        code |= ch;
        if (count < 4) {
            code <<= 8;
        }
        count++;
    }

    *length = count;

    return code;
}

int find_info_bits(long code)
{
    int word = 0x0;
    int word_pos = 31;
    int code_shift = 36;

    while (word_pos >= 0) {
        if (code_shift == 35 || code_shift == 31 || code_shift == 23 || code_shift == 7) {
            code_shift--;
            continue;
        }

        if (code & (1L << code_shift)) {
            word |= 1 << word_pos;
        }
        word_pos--;
        code_shift--;
    }

    return word;
}

void output_word(int word)
{
    int shift = 24;
    int mask = 0xFF000000;

    while (shift >= 0) {
        char curr_byte = (word & mask) >> shift;
        putchar(curr_byte);
        shift -= 8;
        mask >>= 8;
    }
}

bool decode(void)
{
    int length = 0;

    while (!feof(stdin)) {
        long code = parse_word_decode(&length);

        if (length == 0) {
            break;
        }

        if (length != 5) {
            fprintf(stderr, "Wrong code word\n");
            return false;
        }

        int word = find_info_bits(code);

        output_word(word);
    }

    return true;
}

/*************************************
 * DO NOT MODIFY THE FUNCTION BELLOW *
 *************************************/
int main(int argc, char *argv[])
{
    if (argc > 2) {
        fprintf(stderr, "Usage: %s (-e|-d)\n", argv[0]);
        return EXIT_FAILURE;
    }
    bool is_success = false;
    if (argc == 1 || (strcmp(argv[1], "-e") == 0)) {
        is_success = encode();
    } else if (strcmp(argv[1], "-d") == 0) {
        is_success = decode();
    } else {
        fprintf(stderr, "Unknown argument: %s\n", argv[1]);
    }
    return is_success ? EXIT_SUCCESS : EXIT_FAILURE;
}
