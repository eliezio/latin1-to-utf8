#ifndef _KMP_H
#define _KMP_H

#ifdef __cplusplus

#include <cstdlib>
#include <cstdint>

class KmpSearch {
public:
    KmpSearch(const uint8_t *pattern, size_t psize);

    ~KmpSearch();

    void reset();

    bool match(uint8_t c);

private:

    int *   pi;
    const uint8_t *pattern;
    size_t  psize;
    int     k;
};

extern "C" {

#endif

int kmp(const char *target, const char *pattern);

#ifdef __cplusplus
}
#endif

#endif
