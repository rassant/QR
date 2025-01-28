/*g++ main.cpp -L/usr/local/lib -lgtest -lgtest_main -pthread -o example -std=c++20*/
#include <gtest/gtest.h>

TEST(ExampleTest, BasicAssertion) {
    EXPECT_EQ(2 + 2, 4);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
