#include "latin1-to-utf8.h"

#include <stdbool.h>
#include <ctype.h>

static const uint8_t *beginText;
static const uint8_t *endText;

static bool  inText;
static const uint8_t *cursor;
static bool  c3;

void beginTranslate (const char *_beginText, const char *_endText) {
    if (_beginText != NULL) {
        inText = false;
        cursor = (beginText = (const uint8_t *) _beginText);
        endText = (const uint8_t *) _endText;
    } else {
        inText = true;
        cursor = beginText = endText = NULL;
    }
    c3 = false;
}

size_t translate(const uint8_t *input, size_t inputLen, uint8_t * const output) {
    uint8_t *out = output;
    for (; inputLen > 0; inputLen--) {
        uint8_t c = *input++;
        if (inText) {
            if (! c3) {
                if (c == 0xc3) {
                    c3 = true;
                } else if (c < 0x80) {
                    *out++ = c;
                // Caso especial: /\BFsr/...
                } else if (c == 0xbf) {
                    *out++ = 'u';
                } else if (c < 0xc0) {
                    *out++ = 0xc2;
                    *out++ = c;
                } else {
                    *out++ = 0xc3;
                    *out++ = c ^ 0x40;
                }
            // Caso especial: fun\C3\C3o
            } else if (c == 0xc3) {
                *out++ = 0xc3;
                *out++ = 0xa7;
                *out++ = 0xc3;
                *out++ = 0xa3;
                c3 = false;
            } else {
                *out++ = 0xc3;
                if (isascii(c)) {
                    *out++ = 0xc3 ^ 0x40;
                }
                *out++ = c;
                c3 = false;
            }
            if (cursor != NULL) {
                if (c == *cursor) {
                    if (*++cursor == '\0') {
                        inText = false;
                        cursor = beginText;
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
                    cursor = beginText;
                    if (c == *cursor) {
                        cursor++;
                    }
                }
            }
        }
    }
    return (out - output);
}

size_t endTranslate(uint8_t *const output) {
    uint8_t *out = output;
    if (c3) {
        *out++ = 0xc3;
        *out++ = 0xc3 ^ 0x40;
    }
    return out - output;;
}
