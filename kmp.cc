/*
   Copyright 2011 Shao-Chuan Wang <shaochuan.wang AT gmail.com>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include "kmp.h"

#include <string.h>

static int * compute_pattern(const uint8_t *pattern, size_t psize);

KmpSearch::KmpSearch(const uint8_t *pattern, size_t psize) {
    this->pattern = pattern;
    this->psize = psize;
    this->pi = compute_pattern(pattern, psize);
    reset();
}

KmpSearch::~KmpSearch() {
    delete [] pi;
}

void KmpSearch::reset() {
    k = -1;
}

bool KmpSearch::match(uint8_t c) {
    while ((k > -1) && (c != pattern[k+1]))
        k = pi[k];
    if (c == pattern[k+1])
        k++;
    return (k == psize - 1);
}

int kmp(const char *target, const char *pattern) {
    const size_t tsize = strlen(target);
    const size_t psize = strlen(pattern);

    KmpSearch kmpSearch = KmpSearch((const uint8_t *) pattern, psize);
    for (int i = 0; i < tsize; i++) {
        if (kmpSearch.match((uint8_t) target[i])) {
            return (int) (i - psize + 1);
        }
    }
    return -1;
}

static int * compute_pattern(const uint8_t *pattern, size_t psize) {
    int k = -1;
    int *pi = new int[sizeof(int) * psize];
    pi[0] = k;
    for (int i = 1; i < psize; i++) {
        while ((k > -1) && (pattern[k+1] != pattern[i]))
            k = pi[k];
        if (pattern[i] == pattern[k+1])
            k++;
        pi[i] = k;
    }
    return pi;
}
