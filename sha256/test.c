#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "sha256.h"

typedef struct {
    u_int8_t input[2 * 4096];
    u_int64_t len;
    u_int32_t expected[8];
} test_case_t;

bool test(const u_int32_t *actual, const u_int32_t *expected) {
    for (int i = 0; i < 8; i++) {
        if (actual[i] != expected[i]) {
            return false;
        }
    }
    return true;
}

int main() {
    const int nb_tests = 4;
    const test_case_t tests[] = {
        {
            .input = {},
            .len = 0,
            .expected = {0xe3b0c442, 0x98fc1c14, 0x9afbf4c8, 0x996fb924, 0x27ae41e4, 0x649b934c,
                         0xa495991b, 0x7852b855},
        },
        {
            .input = "foo",
            .len = 3,
            .expected = {0x2c26b46b, 0x68ffc68f, 0xf99b453c, 0x1d304134, 0x13422d70, 0x6483bfa0,
                         0xf98a5e88, 0x6266e7ae},
        },
        {
            .input = "hello world!",
            .len = 12,
            .expected = {0x7509e5bd, 0xa0c762d2, 0xbac7f90d, 0x758b5b22, 0x63fa01cc, 0xbc542ab5,
                         0xe3df163b, 0xe08e6ca9},
        },
        {
            .input = "Vivamus sollicitudin ipsum tellus, nec pulvinar tellus suscipit non. In at "
                     "tincidunt ante. Cras vel mauris purus. Donec a quam ac leo convallis "
                     "facilisis et ultrices libero. Suspendisse varius, nunc quis egestas "
                     "facilisis, tortor nisl mattis lacus, id vel.",
            .len = 256,
            .expected = {0x72e07d17, 0x5094ed16, 0x4623bc25, 0x030d6ccb, 0x0d735495, 0xf7fec7e4,
                         0x1c017706, 0x8ecee54e},
        }};

    bool ok = true;
    for (int i = 0; i < nb_tests; i++) {
        u_int32_t *actual = sha256_hash(tests[i].len, tests[i].input);
        if (!test(actual, tests[i].expected)) {
            ok = false;
            printf("%d ", i);
            for (int h = 0; h < 8; h++) {
                printf("%08x", actual[h]);
            }
            printf(" ");
            for (int h = 0; h < 8; h++) {
                printf("%08x", tests[i].expected[h]);
            }
        }
    }

    if (ok) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
