#include <gtest/gtest.h>

#include "KmpSearch.h"

TEST(KmpTest, WikiExample)
{
    const char *target = "ABC ABCDAB ABCDABCDABDE";
    const char *pattern = "ABCDABD";

    EXPECT_EQ(15, kmp(target, pattern));
}
