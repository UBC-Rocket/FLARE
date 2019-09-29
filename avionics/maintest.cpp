//
// Created by Quinn Ferbers on 9/29/2019.
// This file currently only runs an example test
//

#include <gtest/gtest.h>

//this empty class can be used to construct data and pre-reqs for certain tests
class MainTest : public ::testing::Test {
};

//an example of a very simple test case. A function could be run inside the
//assert instead
TEST(MainTest, exampleTest) {
    ASSERT_TRUE(1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
