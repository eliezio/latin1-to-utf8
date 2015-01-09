#include "l1u8recode.h"

#include <stdbool.h>
#include <string.h>

#define ESC1    0xc2
#define ESC2    0xc3

#define a_tilde     0xe3
#define c_cedilla   0xe7

/*
 * The general rules to recode character 'ZZ' from Latin1 to UTF8 are as follow:
 *
 * 00 .. 7f    ZZ
 * 80 .. bf    0xC2 ZZ
 * c0 .. ff    0xC3 (ZZ ^ 0x40)
 *
 * You can use the following script to see how all characters are converted:
 *
#!/bin/bash

for ((i = 0; $i < 256; i++)); do
    n=$(printf "%02X" $i)
    u8=$(echo -ne "\x$n" | recode l1..u8/x)
    echo "0x$n -> $u8"
done
 *
 */

L1U8Recode::L1U8Recode() : L1U8Recode(nullptr, nullptr) {
}

L1U8Recode::L1U8Recode(const char *beginText, const char *endText) :
        L1U8Recode((const uint8_t *) beginText, beginText ? strlen(beginText) : 0,
                   (const uint8_t*) endText, endText ? strlen(endText) : 0) {
}

L1U8Recode::L1U8Recode(uint8_t const *beginText, size_t beginTextLen, uint8_t const *endText, size_t endTextLen) {
    kmpSearches[false] = beginText ? new KmpSearch(beginText, beginTextLen) : nullptr;
    kmpSearches[true] = endText ? new KmpSearch(endText, endTextLen) : nullptr;
    init();
}

L1U8Recode::~L1U8Recode() {
    delete kmpSearches[0];
    delete kmpSearches[1];
}

void L1U8Recode::init() {
    if (kmpSearches[false]) {
        kmpSearches[false]->reset();
        this->inText = false;
    } else {
        this->inText = true;
    }
    if (kmpSearches[true]) {
        kmpSearches[true]->reset();
    }
    this->u8Esc = 0;
    this->changesCount = 0;
}

size_t L1U8Recode::translate(const uint8_t *input, size_t inputLen, uint8_t *const output) {
    uint8_t *out = output;
    for (; inputLen > 0; inputLen--) {
        uint8_t c = *input++;
        if (inText) {
            if (! u8Esc) {
                if ((c == ESC1) || (c == ESC2)) {
                    u8Esc = c;
                } else if (c < 0x80) {
                    *out++ = c;
                } else if (c < 0xc0) {
                    *out++ = ESC1;
                    *out++ = c;
                } else {
                    *out++ = ESC2;
                    *out++ = (uint8_t) (c ^ 0x40);
                    ++changesCount;
                }
            } else {
                if (c < 0x80) {
                    *out++ = ESC2;
                    *out++ = (uint8_t) (u8Esc ^ 0x40);
                    ++changesCount;
                    *out++ = c;
                } else if (c < 0xc0) {
                    *out++ = u8Esc;
                    *out++ = c;
                } else {
                    uint8_t c1, c2;
                    // I found many of this weird case on my RCS files with no apparent cause
                    if ((u8Esc == ESC2) && (c == ESC2)) {
                        c1 = c_cedilla;
                        c2 = a_tilde;
                    } else {
                        c1 = u8Esc;
                        c2 = c;
                    }
                    *out++ = ESC2;
                    *out++ = (uint8_t) (c1 ^ 0x40);
                    ++changesCount;
                    *out++ = ESC2;
                    *out++ = (uint8_t) (c2 ^ 0x40);
                    ++changesCount;
                }
                u8Esc = 0;
            }
        } else {
            *out++ = c;
        }
        if (kmpSearches[inText]) {
            if (kmpSearches[inText]->match(c)) {
                inText = ! inText;
                if (kmpSearches[inText]) {
                    kmpSearches[inText]->reset();
                }
            }
        }
    }
    return (size_t) (out - output);
}

size_t L1U8Recode::finish(uint8_t *output) {
    uint8_t *out = output;
    if (u8Esc) {
        *out++ = ESC2;
        *out++ = (uint8_t) (u8Esc ^ 0x40);
    }
    return (size_t) (out - output);
}
