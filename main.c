#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define BLOCK_SIZE 64
#define SCHEDULE_SIZE 64

void print_block(u_int8_t block[BLOCK_SIZE]) {
    for (u_int8_t i = 0; i < BLOCK_SIZE; i += 4) {
        printf("%02x-%02x: %08b %08b %08b %08b\n", i, i + 3, block[i], block[i + 1], block[i + 2],
               block[i + 3]);
    }
}

void print_schedule(u_int32_t schedule[SCHEDULE_SIZE]) {
    for (u_int8_t i = 0; i < SCHEDULE_SIZE; i++) {
        printf("%2d: %032b\n", i, schedule[i]);
    }
}

u_int32_t ror(u_int32_t x, u_int32_t n) {
    return (x >> n) | (x << (32 - n));
}

int main(int argc, char *argv[]) {
    u_int8_t block[64] = {0};
    block[0] = 0b10100101;
    block[1] = 0b01011010;
    block[2] = 0b00001010;
    block[3] = 0b10000000;
    block[63] = 24;

    u_int32_t w[SCHEDULE_SIZE] = {0};
    for (u_int8_t i = 0; i < 16; i++) {
        w[i] = block[4 * i] << 24 | block[4 * i + 1] << 16 | block[4 * i + 2] << 8 |
               block[4 * i + 3] << 0;
    }

    print_block(block);
    print_schedule(w);

    for (int i = 16; i < SCHEDULE_SIZE; i++) {
        u_int8_t i0 = i - 15;
        u_int8_t i1 = i - 2;
        u_int32_t s0 = ror(w[i0], 7) ^ ror(w[i0], 18) ^ (w[i0] >> 3);
        u_int32_t s1 = ror(w[i1], 17) ^ ror(w[i1], 19) ^ (w[i1] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    print_schedule(w);

    return EXIT_SUCCESS;
}
