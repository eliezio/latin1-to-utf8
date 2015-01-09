#ifndef L1U8RECODE_H
#define L1U8RECODE_H

#include <cstdint>
#include <cstdlib>

#include "KmpSearch.h"

class L1U8Recode {
public:
    L1U8Recode();

    L1U8Recode(const char *beginText, const char *endText);

    L1U8Recode(const uint8_t *beginText, size_t beginTextLen, const uint8_t *endText, size_t endTextLen);

    ~L1U8Recode();

    void init();

    size_t translate(const uint8_t *input, size_t inputLen, uint8_t * const output);

    size_t finish(uint8_t *output);

    size_t getChangesCount() const {
        return changesCount;
    }

private:
    KmpSearch * kmpSearches[2];
    bool        inText;
    uint8_t     u8Esc;
    size_t      changesCount;
};

#endif
