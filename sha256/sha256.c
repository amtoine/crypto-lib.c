#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "sha256.h"

#define BLOCK_SIZE 64
#define SCHEDULE_SIZE 64

u_int32_t ror(u_int32_t x, u_int32_t n) {
    return (x >> n) | (x << (32 - n));
}

// first 32 bits of the fractional parts of the cube roots of the first 64 primes 2..311
const u_int32_t k[64] = {
    0b01000010100010100010111110011000, 0b01110001001101110100010010010001,
    0b10110101110000001111101111001111, 0b11101001101101011101101110100101,
    0b00111001010101101100001001011011, 0b01011001111100010001000111110001,
    0b10010010001111111000001010100100, 0b10101011000111000101111011010101,
    0b11011000000001111010101010011000, 0b00010010100000110101101100000001,
    0b00100100001100011000010110111110, 0b01010101000011000111110111000011,
    0b01110010101111100101110101110100, 0b10000000110111101011000111111110,
    0b10011011110111000000011010100111, 0b11000001100110111111000101110100,
    0b11100100100110110110100111000001, 0b11101111101111100100011110000110,
    0b00001111110000011001110111000110, 0b00100100000011001010000111001100,
    0b00101101111010010010110001101111, 0b01001010011101001000010010101010,
    0b01011100101100001010100111011100, 0b01110110111110011000100011011010,
    0b10011000001111100101000101010010, 0b10101000001100011100011001101101,
    0b10110000000000110010011111001000, 0b10111111010110010111111111000111,
    0b11000110111000000000101111110011, 0b11010101101001111001000101000111,
    0b00000110110010100110001101010001, 0b00010100001010010010100101100111,
    0b00100111101101110000101010000101, 0b00101110000110110010000100111000,
    0b01001101001011000110110111111100, 0b01010011001110000000110100010011,
    0b01100101000010100111001101010100, 0b01110110011010100000101010111011,
    0b10000001110000101100100100101110, 0b10010010011100100010110010000101,
    0b10100010101111111110100010100001, 0b10101000000110100110011001001011,
    0b11000010010010111000101101110000, 0b11000111011011000101000110100011,
    0b11010001100100101110100000011001, 0b11010110100110010000011000100100,
    0b11110100000011100011010110000101, 0b00010000011010101010000001110000,
    0b00011001101001001100000100010110, 0b00011110001101110110110000001000,
    0b00100111010010000111011101001100, 0b00110100101100001011110010110101,
    0b00111001000111000000110010110011, 0b01001110110110001010101001001010,
    0b01011011100111001100101001001111, 0b01101000001011100110111111110011,
    0b01110100100011111000001011101110, 0b01111000101001010110001101101111,
    0b10000100110010000111100000010100, 0b10001100110001110000001000001000,
    0b10010000101111101111111111111010, 0b10100100010100000110110011101011,
    0b10111110111110011010001111110111, 0b11000110011100010111100011110010,
};

u_int32_t *sha256_hash(u_int64_t size, const u_int8_t *input) {
    // FIXME: when the size of the input data is really close to the max value for 64-bit integers
    u_int64_t nb_blocks = (size + 8) / BLOCK_SIZE + 1;

    u_int8_t *blocks = calloc(nb_blocks * BLOCK_SIZE, sizeof(u_int8_t));
    if (blocks == NULL) {
        return NULL;
    }
    for (u_int64_t i = 0; i < size; i++) {
        blocks[i] = input[i];
    }
    blocks[size] = 0x80;
    for (int i = 0; i < 8; i++) {
        blocks[nb_blocks * BLOCK_SIZE - i - 1] = 8 * (size >> (i * 8));
    }

    u_int32_t *hash = malloc(8 * sizeof(u_int32_t));
    if (hash == NULL) {
        return NULL;
    }
    // first 32 bits of the fractional parts of the square roots of the first 8 primes 2..19
    hash[0] = 0b01101010000010011110011001100111;
    hash[1] = 0b10111011011001111010111010000101;
    hash[2] = 0b00111100011011101111001101110010;
    hash[3] = 0b10100101010011111111010100111010;
    hash[4] = 0b01010001000011100101001001111111;
    hash[5] = 0b10011011000001010110100010001100;
    hash[6] = 0b00011111100000111101100110101011;
    hash[7] = 0b01011011111000001100110100011001;

    u_int32_t w[SCHEDULE_SIZE] = {0};

    for (u_int64_t i = 0; i < nb_blocks; i++) {
        for (u_int8_t j = 0; j < 16; j++) {
            u_int64_t offset = i * BLOCK_SIZE + 4 * j;
            w[j] = blocks[offset] << 24 | blocks[offset + 1] << 16 | blocks[offset + 2] << 8 |
                   blocks[offset + 3] << 0;
        }

        for (u_int8_t j = 16; j < SCHEDULE_SIZE; j++) {
            u_int8_t i0 = j - 15;
            u_int8_t i1 = j - 2;
            u_int32_t s0 = ror(w[i0], 7) ^ ror(w[i0], 18) ^ (w[i0] >> 3);
            u_int32_t s1 = ror(w[i1], 17) ^ ror(w[i1], 19) ^ (w[i1] >> 10);
            w[j] = w[j - 16] + s0 + w[j - 7] + s1;
        }

        u_int32_t a = hash[0], b = hash[1], c = hash[2], d = hash[3], e = hash[4], f = hash[5],
                  g = hash[6], h = hash[7];

        for (u_int8_t j = 1; j < 65; j++) {
            u_int32_t s1 = ror(e, 6) ^ ror(e, 11) ^ ror(e, 25);
            u_int32_t choice = (e & f) ^ (~e & g);
            u_int32_t s0 = ror(a, 2) ^ ror(a, 13) ^ ror(a, 22);
            u_int32_t majority = (a & b) ^ (a & c) ^ (b & c);

            u_int32_t tmp_1 = h + s1 + choice + k[j - 1] + w[j - 1];
            u_int32_t tmp_2 = s0 + majority;

            h = g;
            g = f;
            f = e;
            e = d + tmp_1;
            d = c;
            c = b;
            b = a;
            a = tmp_1 + tmp_2;
        }

        hash[0] += a;
        hash[1] += b;
        hash[2] += c;
        hash[3] += d;
        hash[4] += e;
        hash[5] += f;
        hash[6] += g;
        hash[7] += h;
    }

    free(blocks);

    return hash;
}
