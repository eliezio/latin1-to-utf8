
#include "l1u8recode.h"
#include "config.h"

#include <memory>
#include <cstring>
#include <iostream>
#include <fstream>

#include <popt.h>
#include <unistd.h>

enum {
    CL_OPT_RCS_LOG = 1,
    CL_OPT_VERSION,
};

static char *    beginText = nullptr;
static char *    endText = nullptr;
static char *    inputFileName = nullptr;
static char *    outputFileName = nullptr;
static int       verbose = false;
static int       xargsMode = false;

static const struct poptOption options[] = {
    {
        "version",
        0,
        POPT_ARG_NONE,
        NULL,
        CL_OPT_VERSION,
        "Show version",
        NULL,
    },
    {
        "--verbose",
        'v',
        POPT_ARG_NONE,
        &verbose,
        0,
        "Enable verbose mode",
        NULL,
    },
    {
        "in",
        0,
        POPT_ARG_STRING,
        &inputFileName,
        0,
        "Input file (default: stdin)",
        "FILENAME",
    },
    {
        "out",
        0,
        POPT_ARG_STRING,
        &outputFileName,
        0,
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

static inline bool stringEquals(const char *str1, const char *str2);

typedef void (*CustomDeleter)(void *);

static std::unique_ptr<char[], CustomDeleter> realPathOf(const char *path) {
    std::unique_ptr<char[], CustomDeleter> realPath(realpath(path, nullptr), free);

    return realPath;
}

/**
* The basic recode method, using standard I/O abstractions.
*/
static size_t recode(std::istream &in, std::ostream &out);

/**
* Perform a recoding
*/
static bool recode(const char *inputFileName, const char *outputFileName);

static inline bool recode(std::string &inputFileName) {
    return recode(inputFileName.c_str(), nullptr);
}

int main(int argc, char *argv[]) {
    poptContext     optCtx;
    int             c;

    optCtx = poptGetContext("l1u8recode", argc, (const char **) argv, options, 0);
    while ((c = poptGetNextOpt(optCtx)) > 0) {
        switch (c) {
            case CL_OPT_RCS_LOG:
                beginText = (char *) RCS_LOG_BEGIN;
                endText = (char *) RCS_LOG_END;
                break;

            case CL_OPT_VERSION:
                std::cout << "l1u8recode version " << VERSION << std::endl
                        << "Copyright (C) 2015 Eliezio Oliveira" << std::endl
                        << std::endl
                        << "This is free software; see the source for copying conditions.  There is NO" << std::endl
                        << "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << std::endl
                        << std::endl
                        << "Written by Eliezio Oliveira <ebo@pobox.com> " << std::endl;
                return EXIT_SUCCESS;

            default:
                break;
        }
    }
    if (c < -1) {
        std::cerr << poptBadOption(optCtx, POPT_BADOPTION_NOALIAS) << ": " << poptStrerror(c) << std::endl;
        return EXIT_FAILURE;
    }
    poptFreeContext(optCtx);
    bool success = true;
    if (xargsMode) {
        if (inputFileName || outputFileName) {
            std::cerr << "--xargs is mutually exclusive with --in/--out options" << std::endl;
            return EXIT_FAILURE;
        }
        std::string fileName;
        while (std::getline(std::cin, fileName)) {
            if (!fileName.empty()) {
                success = success && recode(fileName);
            }
        }
    } else {
        std::unique_ptr<char[], CustomDeleter> realInputPath = realPathOf(inputFileName);
        std::unique_ptr<char[], CustomDeleter> realOutputPath = realPathOf(outputFileName);
        if (realInputPath.get() && stringEquals(realInputPath.get(), realOutputPath.get())) {
            std::cerr << "Input and output files cannot be the same" << std::endl;
            return EXIT_FAILURE;
        }
        success = recode(inputFileName, outputFileName);
    }
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

#define CHUNK_SIZE     2000

size_t recode(std::istream &in, std::ostream &out) {
    uint8_t         input[CHUNK_SIZE];
    uint8_t         output[2 * CHUNK_SIZE];
    size_t          n;
    L1U8Recode      l1U8Recode = L1U8Recode(beginText, endText);

    while (!in.eof()) {
        in.read((char *) input, sizeof(input));
        n = l1U8Recode.translate(input, (size_t) in.gcount(), output);
        if (n > 0) {
            out.write((const char *) output, (long) n);
        }
    }
    n = l1U8Recode.finish(output);
    if (n > 0) {
        out.write((const char *) output, (long) n);
    }
    return l1U8Recode.getChangesCount();
}

bool recode(const char *inputFileName, const char *outputFileName) {
    std::string tempOutputFileName;
    bool        inPlaceRecoding = false;
    std::istream *in;
    std::ifstream ifs;
    if (inputFileName) {
        ifs.open(inputFileName, std::ifstream::binary);
        if (ifs.fail()) {
            std::cerr << "Error on opening input file '" << inputFileName << "': " << strerror(errno) << std::endl;
            return false;
        }
        in = &ifs;
        if (!outputFileName) {
            inPlaceRecoding = true;
            tempOutputFileName = inputFileName;
            tempOutputFileName += "!recode";
            outputFileName = tempOutputFileName.c_str();
        }
    } else {
        in = &std::cin;
    }
    std::ostream *log;
    std::ostream *out;
    std::ofstream ofs;
    if (outputFileName) {
        ofs.open(outputFileName, std::ofstream::binary | std::ofstream::trunc);
        if (ofs.fail()) {
            std::cerr << "Error on opening output file '" << outputFileName << "': " << strerror(errno) << std::endl;
            return false;
        }
        out = &ofs;
        log = &std::cout;
    } else {
        out = &std::cout;
        log = &std::cerr;
    }
    if (verbose && inputFileName) {
        (*log) << "Recoding file '" << inputFileName << "'... ";
    }
    auto changesCount = recode(*in, *out);
    if (verbose && inputFileName) {
        (*log) << changesCount << " characters recoded." << std::endl;
    }
    if (inPlaceRecoding) {
        (void) unlink(inputFileName);
        int rc = rename(outputFileName, inputFileName);
        if (rc < 0) {
            std::cerr << "Error on renaming '" << outputFileName << "' to '" << inputFileName << "': " << strerror(errno) << std::endl;
            return false;
        }
    }
    return true;
}

bool static inline stringEquals(const char *str1, const char *str2) {
    if (!str1 && !str2) {
        return true;
    } else if (!str1 || !str2) {
        return false;
    } else {
        return strcmp(str1, str2) == 0;
    }
}
