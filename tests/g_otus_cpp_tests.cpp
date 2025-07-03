#include <iostream>
#include <sstream>
#include <gtest/gtest.h>
#include "lib.h"

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(OtusCppTest, checkVersion) {  
    int ver_num = version(); 
    EXPECT_GT(ver_num, 0);
}
