#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "latin1-to-utf8.h"

#define MAX_LINE_LENGTH     2000


int main(int argc, char *argv[]) {
    static uint8_t input[MAX_LINE_LENGTH];
    static uint8_t output[2 * MAX_LINE_LENGTH];
    size_t n;

    if ((argc > 1) && (strcmp(argv[1], "--rcs") == 0)) {
        beginTranslate("\nlog\n", "\n@\n");
    } else {
        beginTranslate(NULL, NULL);
    }

    while (! feof(stdin)) {
        n = fread(input, 1, sizeof(input), stdin);
        n = translate(input, n, output);
        if (n > 0) {
            fwrite(output, 1, n, stdout);
        }
    }
    n = endTranslate(output);
    if (n > 0) {
        fwrite(output, 1, n, stdout);
    }

    return EXIT_SUCCESS;
}
