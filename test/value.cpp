#include "value.hpp"
#include "gtest/gtest.h"
using namespace nscheme;

TEST(ValueTest, Integer)
{
    Value v = Value::fromInteger(42);
    EXPECT_TRUE(v.isInteger());
    EXPECT_FALSE(v.isPointer());
    EXPECT_FALSE(v.isSymbol());
    EXPECT_FALSE(v.isCharacter());
    EXPECT_EQ(42, v.asInteger());
    EXPECT_EQ("42", v.toString());
}
