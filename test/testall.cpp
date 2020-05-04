#include "version.h"
#include <gtest/gtest.h>
#include <iostream>

TEST(testall, testversion) 
{
    const char * first = "Hello, World!";
    const char * second = "hello, world!";

    // EXPECT_STREQ(first, second);
    EXPECT_STRNE(first, second);
    EXPECT_STRCASEEQ(first, second);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}