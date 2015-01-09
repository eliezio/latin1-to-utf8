#include <iostream>

#include <cstring>
#include <popt.h>

#include "l1u8recode.h"

#define MAX_LINE_LENGTH     2000

enum {
    CL_OPT_RCS_LOG = 1,
    CL_OPT_INPUT,
    CL_OPT_OUTPUT,
};

static char *    beginText = nullptr;
static char *    endText = nullptr;
static char *    fileName = nullptr;
static int       xargsMode = false;

static const struct poptOption options[] = {
    {
        "in",
        0,
        POPT_ARG_STRING,
        &fileName,
        CL_OPT_INPUT,
        "Input file (default: stdin)",
        "FILENAME",
    },
    {
        "out",
        0,
        POPT_ARG_STRING,
        &fileName,
        CL_OPT_OUTPUT,
        "Output file (default: stdout)",
        "FILENAME",
    },
    {
        "xargs",
        0,
        POPT_ARG_NONE,
        &xargsMode,
        0,
        "Read file names from stdin and recode them in-place",
        NULL
    },
    {
        "begin",
        0,
        POPT_ARG_STRING,
        &beginText,
        0,
        "Recode only sections beginning with PATTERN. Used in conjunction with --end",
        "PATTERN",
    },
    {
        "end",
        0,
        POPT_ARG_STRING,
        &endText,
        0,
        "Recode only sections ending with PATTERN. Used in conjunction with --begin",
        "PATTERN",
    },
    {
        "rcs-log",
        0,
        POPT_ARG_NONE,
        NULL,
        CL_OPT_RCS_LOG,
        "Recode only log messages in a RCS formatted file",
        NULL,
    },
    POPT_AUTOHELP
    POPT_TABLEEND
};

static const char *RCS_LOG_BEGIN = "\nlog\n";
static const char *RCS_LOG_END = "\n@\n";

static inline bool strequal(const char *str1, const char *str2) {
    if (!str1 && !str2) {
        return true;
    } else if (!str1 || !str2) {
        return false;
    } else {
        return strcmp(str1, str2) == 0;
    }
}

int main(int argc, char *argv[]) {
    poptContext     optCtx;
    int             c;
    char *          inputFileName = nullptr;
    char *          outputFileName = nullptr;
    static uint8_t  input[MAX_LINE_LENGTH];
    static uint8_t  output[2 * MAX_LINE_LENGTH];
    FILE *          in;
    FILE *          out;
    size_t          n;

    optCtx = poptGetContext("l1u8recode", argc, (const char **) argv, options, 0);
    while ((c = poptGetNextOpt(optCtx)) > 0) {
        switch (c) {
            case CL_OPT_RCS_LOG:
                beginText = (char *) RCS_LOG_BEGIN;
                endText = (char *) RCS_LOG_END;
                break;

            case CL_OPT_INPUT:
                free(inputFileName);
                inputFileName = realpath(fileName, nullptr);
                break;

            case CL_OPT_OUTPUT:
                free(outputFileName);
                outputFileName = realpath(fileName, nullptr);
                break;

            default:
                break;
        }
    }
    if (c < -1) {
        std::cerr << poptBadOption(optCtx, POPT_BADOPTION_NOALIAS) << ": " << poptStrerror(c) << std::endl;
        return EXIT_FAILURE;
    }
    if (inputFileName && outputFileName && strequal(inputFileName, outputFileName)) {
        std::cerr << "Input and output cannot be the same" << std::endl;
        return EXIT_FAILURE;
    }
    if (inputFileName) {
        in = fopen(inputFileName, "r");
        if (!in) {
            std::cerr << "Error on opening input file '" << inputFileName << "': " << strerror(errno) << std::endl;
            return EXIT_FAILURE;
        }
        free(inputFileName);
    } else {
        in = stdin;
    }
    if (outputFileName) {
        out = fopen(outputFileName, "w+");
        if (!out) {
            std::cerr << "Error on opening output file '" << outputFileName << "': " << strerror(errno) << std::endl;
            return EXIT_FAILURE;
        }
        free(outputFileName);
    } else {
        out = stdout;
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
