#include "object.hpp"
#include <gtest/gtest.h>

using namespace nscheme;

TEST(BooleanObject, ToString) {
    BooleanObject obj1(true);
    EXPECT_EQ("#t", ObjectToString(&obj1));
    BooleanObject obj2(false);
    EXPECT_EQ("#f", ObjectToString(&obj2));
}

TEST(ByteVectorObject, ToString) {
    ByteVectorObject obj1(3, 42);
    EXPECT_EQ("#u8(42 42 42)", ObjectToString(&obj1));
}

TEST(CharacterObject, ToString) {
    CharacterObject obj1('a');
    EXPECT_EQ("#\\a", ObjectToString(&obj1));
    CharacterObject obj2(' ');
    EXPECT_EQ("#\\x20", ObjectToString(&obj2));
}

TEST(PairObject, ToString1) {
    NilObject nil;
    IntegerObject i1(1);
    IntegerObject i2(2);
    IntegerObject i3(3);
    PairObject obj1(&i3, &nil);
    PairObject obj2(&i2, &obj1);
    PairObject obj3(&i1, &obj2);
    EXPECT_EQ("(1 2 3)", ObjectToString(&obj3));
}

TEST(PairObject, ToString2) {
    IntegerObject i0(0);
    IntegerObject i1(1);
    IntegerObject i2(2);
    IntegerObject i3(3);
    PairObject obj1(&i3, &i0);
    PairObject obj2(&i2, &obj1);
    PairObject obj3(&i1, &obj2);
    EXPECT_EQ("(1 2 3 . 0)", ObjectToString(&obj3));
}

TEST(StringObject, ToString1) {
    StringObject obj("hello");
    EXPECT_EQ("\"hello\"", ObjectToString(&obj));
}

TEST(StringObject, ToString2) {
    StringObject obj("hello\"world");
    EXPECT_EQ("\"hello\\\"world\"", ObjectToString(&obj));
}

TEST(StringObject, ToString3) {
    StringObject obj("hello\x01world");
    EXPECT_EQ("\"hello\\x01world\"", ObjectToString(&obj));
}
