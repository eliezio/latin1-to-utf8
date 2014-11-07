#ifndef LATIN1_TO_UTF8_H
#define LATIN1_TO_UTF8_H

#include <inttypes.h>
#include <stdlib.h>

void beginTranslate (const char *beginText, const char *endText);

size_t translate(const uint8_t *input, size_t inputLen, uint8_t *output);

size_t endTranslate(uint8_t *output);

#endif
