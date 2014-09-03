#include "object.hpp"
#include "object_list.hpp"
#include <gtest/gtest.h>

using namespace nscheme;

ObjectList gList;

template <typename T, typename ...Args>
Object* New(Args&& ...args) {
    return NewObject<T>(&gList, std::forward<Args>(args)...);
}

TEST(BooleanObject, ToString) {
    Object* obj1 = GetBooleanObject(true);
    EXPECT_EQ("#t", ObjectToString(obj1));
    Object* obj2 = GetBooleanObject(false);
    EXPECT_EQ("#f", ObjectToString(obj2));
}

TEST(ByteVectorObject, ToString) {
    Object* obj1 = New<ByteVectorObject>(3, 42);
    EXPECT_EQ("#u8(42 42 42)", ObjectToString(obj1));
}

TEST(CharacterObject, ToString) {
    Object* obj1 = GetCharacterObject('a');
    EXPECT_EQ("#\\a", ObjectToString(obj1));
    Object* obj2 = GetCharacterObject(' ');
    EXPECT_EQ("#\\x20", ObjectToString(obj2));
}

TEST(PairObject, ToString1) {
    Object* nil = GetNilObject();
    Object* i1 = GetIntegerObject(1LL);
    Object* i2 = GetIntegerObject(2LL);
    Object* i3 = GetIntegerObject(3LL);
    Object* obj1 = New<PairObject>(i3, nil);
    Object* obj2 = New<PairObject>(i2, obj1);
    Object* obj3 = New<PairObject>(i1, obj2);
    EXPECT_EQ("(1 2 3)", ObjectToString(obj3));
}

TEST(PairObject, ToString2) {
    Object* i0 = GetIntegerObject(0LL);
    Object* i1 = GetIntegerObject(1LL);
    Object* i2 = GetIntegerObject(2LL);
    Object* i3 = GetIntegerObject(3LL);
    Object* obj1 = New<PairObject>(i3, i0);
    Object* obj2 = New<PairObject>(i2, obj1);
    Object* obj3 = New<PairObject>(i1, obj2);
    EXPECT_EQ("(1 2 3 . 0)", ObjectToString(obj3));
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
