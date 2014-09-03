#include "object.hpp"
#include "object_list.hpp"
#include <gtest/gtest.h>

using namespace nscheme;

ObjectList gList;

template <typename T, typename ...Args>
ObjectRef New(Args&& ...args) {
    return ref_cast(NewObject<T>(&gList, std::forward<Args>(args)...));
}

TEST(BooleanObject, ToString) {
    ObjectRef obj1 = GetBooleanObject(true);
    EXPECT_EQ("#t", ObjectToString(obj1));
    ObjectRef obj2 = GetBooleanObject(false);
    EXPECT_EQ("#f", ObjectToString(obj2));
}

TEST(ByteVectorObject, ToString) {
    ObjectRef obj1 = New<ByteVectorObject>(3, 42);
    EXPECT_EQ("#u8(42 42 42)", ObjectToString(obj1));
}

TEST(CharacterObject, ToString) {
    ObjectRef obj1 = GetCharacterObject('a');
    EXPECT_EQ("#\\a", ObjectToString(obj1));
    ObjectRef obj2 = GetCharacterObject(' ');
    EXPECT_EQ("#\\x20", ObjectToString(obj2));
}

TEST(PairObject, ToString1) {
    ObjectRef nil = GetNilObject();
    ObjectRef i1 = GetIntegerObject(1LL);
    ObjectRef i2 = GetIntegerObject(2LL);
    ObjectRef i3 = GetIntegerObject(3LL);
    ObjectRef obj1 = New<PairObject>(i3, nil);
    ObjectRef obj2 = New<PairObject>(i2, obj1);
    ObjectRef obj3 = New<PairObject>(i1, obj2);
    EXPECT_EQ("(1 2 3)", ObjectToString(obj3));
}

TEST(PairObject, ToString2) {
    ObjectRef i0 = GetIntegerObject(0LL);
    ObjectRef i1 = GetIntegerObject(1LL);
    ObjectRef i2 = GetIntegerObject(2LL);
    ObjectRef i3 = GetIntegerObject(3LL);
    ObjectRef obj1 = New<PairObject>(i3, i0);
    ObjectRef obj2 = New<PairObject>(i2, obj1);
    ObjectRef obj3 = New<PairObject>(i1, obj2);
    EXPECT_EQ("(1 2 3 . 0)", ObjectToString(obj3));
}

TEST(StringObject, ToString1) {
    ObjectRef obj = New<StringObject>("hello");
    EXPECT_EQ("\"hello\"", ObjectToString(obj));
}

TEST(StringObject, ToString2) {
    ObjectRef obj = New<StringObject>("hello\"world");
    EXPECT_EQ("\"hello\\\"world\"", ObjectToString(obj));
}

TEST(StringObject, ToString3) {
    ObjectRef obj = New<StringObject>("hello\x01world");
    EXPECT_EQ("\"hello\\x01world\"", ObjectToString(obj));
}
