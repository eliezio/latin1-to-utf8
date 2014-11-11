#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#include "l1u8recode.h"

#define MAX_LINE_LENGTH     2000

static char const *const RCS_LOG_BEGIN = "\nlog\n";
static char const *const RCS_LOG_END = "\n@\n";

int main(int argc, char *argv[]) {
    static uint8_t  input[MAX_LINE_LENGTH];
    static uint8_t  output[2 * MAX_LINE_LENGTH];
    const char *    beginText = nullptr;
    const char *    endText = nullptr;
    FILE *          in = stdin;
    FILE *          out = stdout;
    size_t          n;

    for (;;) {
        static struct option long_options[] = {
                { "rcs-log", no_argument, nullptr, 'R'},
                { "begin", required_argument, nullptr, 'b'},
                { "end", required_argument, nullptr, 'e'},
                { "in", required_argument, nullptr, 'i'},
                { "out", required_argument, nullptr, 'o'},
                { nullptr, 0, nullptr, 0 }
        };
        int option_index = 0;
        int c = getopt_long(argc, argv, "Rb:e:i:o:", long_options, &option_index);
        if (c < 0) {
            break;
        }
        switch (c) {
            case 'R':
                beginText = RCS_LOG_BEGIN;
                endText = RCS_LOG_END;
                break;

            case 'b':
                beginText = optarg;
                break;

            case 'e':
                endText = optarg;
                break;

            case 'i':
                in = fopen(optarg, "r");
                if (in == nullptr) {
                    fprintf(stderr, "Error on opening input file '%s': %s\n", optarg, strerror(errno));
                    return EXIT_FAILURE;
                }
                break;

            case 'o':
                out = fopen(optarg, "w+");
                if (out == nullptr) {
                    fprintf(stderr, "Error on opening output file '%s': %s\n", optarg, strerror(errno));
                    return EXIT_FAILURE;
                }
                break;

            default:
                fprintf(stderr, "Invalid syntax\n");
                return EXIT_FAILURE;
        }
    }
    L1U8Recode l1U8Recode = L1U8Recode(beginText, endText);

    while (! feof(in)) {
        n = fread(input, 1, sizeof(input), in);
        n = l1U8Recode.translate(input, n, output);
        if (n > 0) {
            fwrite(output, 1, n, out);
        }
    }
    n = l1U8Recode.finish(output);
    if (n > 0) {
        fwrite(output, 1, n, out);
    }
    fclose(in);
    fclose(out);

    return EXIT_SUCCESS;
}
