#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "sha256.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "expected one positional argument\n");
        exit(EXIT_FAILURE);
    }

    u_int32_t *hash = sha256_hash(strlen(argv[1]), (u_int8_t *)argv[1]);

    printf("%08x%08x%08x%08x%08x%08x%08x%08x\n", hash[0], hash[1], hash[2], hash[3], hash[4],
           hash[5], hash[6], hash[7]);

    return EXIT_SUCCESS;
}
