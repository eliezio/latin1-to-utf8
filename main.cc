#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "l1u8recode.h"

#define MAX_LINE_LENGTH     2000

int main(int argc, char *argv[]) {
    static uint8_t input[MAX_LINE_LENGTH];
    static uint8_t output[2 * MAX_LINE_LENGTH];
    size_t n;
    L1U8Recode l1U8Recode;

    if ((argc > 1) && (strcmp(argv[1], "--rcs") == 0)) {
        l1U8Recode = L1U8Recode("\nlog\n", "\n@\n");
    } else {
        l1U8Recode = L1U8Recode();
    }

    while (! feof(stdin)) {
        n = fread(input, 1, sizeof(input), stdin);
        n = l1U8Recode.translate(input, n, output);
        if (n > 0) {
            fwrite(output, 1, n, stdout);
        }
    }
    n = l1U8Recode.finish(output);
    if (n > 0) {
        fwrite(output, 1, n, stdout);
    }

    return EXIT_SUCCESS;
}
