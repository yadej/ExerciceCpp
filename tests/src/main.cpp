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
}



int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
