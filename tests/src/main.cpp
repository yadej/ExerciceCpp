#include "exerciceCPP/containers/Vector.hpp"
#include <gtest/gtest.h>


TEST(ConstrutorsVector, CheckValues)
{
    yadej::Vector<int> vec = yadej::Vector<int>();
    ASSERT_EQ(vec.size(), 0) << "I hope it is ok";
    EXPECT_TRUE(vec.empty() ) << " Test is empty";
}

TEST(PushAndPopBackVector, TestValues)
{
    yadej::Vector<int> vec = yadej::Vector<int>(4);
    vec.push_back(5);
    EXPECT_EQ(vec[4], 5);
    EXPECT_EQ(vec.size(), 5);
    vec.pop_back();
    vec.pop_back();
    EXPECT_EQ(vec.size(), 3);
    vec.clear();
    EXPECT_EQ(vec.size(), 0);
    EXPECT_TRUE(vec.empty());
}

TEST(InsertVector, TestPosition)
{
    yadej::Vector<int> vec = yadej::Vector<int>(5, 5);
    EXPECT_EQ(vec.size(), 5);
    vec.insert(vec.begin() + 3, 3);
    EXPECT_EQ(3, vec[3]);
    EXPECT_EQ(5, vec[1]);
    EXPECT_EQ(vec.size(), 6);
    int new_value = 8;
    vec.insert(vec.begin() + 2, new_value);
    EXPECT_EQ(vec[2], 8);
    EXPECT_EQ(vec[4], 3);
    yadej::Vector<int> vec2 = yadej::Vector<int>(2, 2);
    vec.insert( vec.begin() + 5, vec2.begin(), vec2.begin() + 2);
    EXPECT_EQ(vec[5], 2);
    EXPECT_EQ(vec[6], 2);

}



int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
