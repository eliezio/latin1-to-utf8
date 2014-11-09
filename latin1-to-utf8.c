#include "latin1-to-utf8.h"

#include <stdbool.h>

#define ESC1    0xc2
#define ESC2    0xc3

#define a_tilde     0xe3
#define c_cedilla   0xe7

static const uint8_t *beginMark;
static const uint8_t *endText;

static bool          inText;
static const uint8_t *cursor;
static uint8_t       u8Esc;

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
void beginTranslate (const char *_beginMark, const char *_endText) {
    if (_beginMark != NULL) {
        inText = false;
        cursor = (beginMark = (const uint8_t *) _beginMark);
        endText = (const uint8_t *) _endText;
    } else {
        inText = true;
        cursor = beginMark = endText = NULL;
    }
    u8Esc = 0;
}

size_t translate(const uint8_t *input, size_t inputLen, uint8_t * const output) {
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
            if (cursor != NULL) {
                if (c == *cursor) {
                    if (*++cursor == '\0') {
                        inText = false;
                        cursor = beginMark;
                    }
                } else if (cursor > endText) {
                    cursor = endText;
                    if (c == *cursor) {
                        cursor++;
                    }
                }
            }
        } else {
            *out++ = c;
            if (cursor != NULL) {
                if (c == *cursor) {
                    if (*++cursor == '\0') {
                        inText = true;
                        cursor = endText;
                    }
                } else {
                    cursor = beginMark;
                    if (c == *cursor) {
                        cursor++;
                    }
                }
            }
        }
    }
    return (size_t) (out - output);
}

size_t endTranslate(uint8_t *output) {
    uint8_t *out = output;
    if (u8Esc) {
        *out++ = ESC2;
        *out++ = (uint8_t) (u8Esc ^ 0x40);
    }
    return (size_t) (out - output);
}
