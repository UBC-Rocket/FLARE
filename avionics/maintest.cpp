//
// Created by Quinn Ferbers on 9/29/2019.
//

#include <gtest/gtest.h>

class MainTest : public ::testing::Test {
};

TEST(MainTest, exampleTest) {
    ASSERT_TRUE(1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
