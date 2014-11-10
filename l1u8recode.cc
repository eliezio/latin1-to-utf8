#include "l1u8recode.h"

#include <stdbool.h>
#include <string.h>

#define ESC1    0xc2
#define ESC2    0xc3

#define a_tilde     0xe3
#define c_cedilla   0xe7

/*
 * General rule to convert from character 'CC' encoded in Latin1 to UTF8:
 * 00 .. 7f    CC
 * 80 .. bf    0xC2 CC
 * c0 .. ff    0xC3 (CC ^ 0x40)
 *
 * The conversion can be shown by the script:
 *
#!/bin/bash

for ((i = 0; $i < 256; i++)); do
    n=$(printf "%02X" $i)
    u8=$(eval "echo -ne '\x$n'" | recode l1..u8/x)
    echo "0x$n -> $u8"
    let i=$i+1
done
 *
 */

L1U8Recode::L1U8Recode() : L1U8Recode(nullptr, nullptr) {
}

}

L1U8Recode::L1U8Recode(const char *beginText, const char *endText) {
    kmpSearches[false] = beginText ? new KmpSearch((const uint8_t *) beginText, strlen(beginText)) : nullptr;
    kmpSearches[true] = endText ? new KmpSearch((const uint8_t *) endText, strlen(endText)) : nullptr;
    init();
}

L1U8Recode::~L1U8Recode() {
    delete kmpSearches[0];
    delete kmpSearches[1];
}

void L1U8Recode::init() {
    if ((this->inText = kmpSearches[false] != nullptr)) {
        kmpSearches[false]->reset();
    }
    if (kmpSearches[true]) {
        kmpSearches[true]->reset();
    }
    this->u8Esc = 0;
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
                }
            } else {
                if (c < 0x80) {
                    *out++ = ESC2;
                    *out++ = (uint8_t) (u8Esc ^ 0x40);
                    *out++ = c;
                } else if (c < 0xc0) {
                    *out++ = u8Esc;
                    *out++ = c;
                } else {
                    uint8_t c1;
                    uint8_t c2;
                    // An abnormal case with an unknown cause
                    if ((u8Esc == ESC2) && (c == ESC2)) {
                        c1 = c_cedilla;
                        c2 = a_tilde;
                    } else {
                        c1 = u8Esc;
                        c2 = c;
                    }
                    *out++ = ESC2;
                    *out++ = (uint8_t) (c1 ^ 0x40);
                    *out++ = ESC2;
                    *out++ = (uint8_t) (c2 ^ 0x40);
                }
                u8Esc = 0;
            }
        } else {
            *out++ = c;
        }
        if (kmpSearches[inText]) {
            if (kmpSearches[inText]->match(c)) {
                inText = ! inText;
                kmpSearches[inText]->reset();
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
