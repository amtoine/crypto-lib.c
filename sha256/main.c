#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "sha256.h"

int main(int argc, char *argv[]) {
    u_int8_t input[] = {'f', 'o', 'o'};
    u_int64_t size = 3;

    u_int32_t *hash = sha256_hash(size, input);

    printf("%08x%08x%08x%08x%08x%08x%08x%08x\n", hash[0], hash[1], hash[2], hash[3], hash[4],
           hash[5], hash[6], hash[7]);

    return EXIT_SUCCESS;
}
