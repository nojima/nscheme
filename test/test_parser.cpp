#include "parser.hpp"

#include <string>
#include <gtest/gtest.h>

#include "scanner.hpp"
#include "object_list.hpp"

using namespace nscheme;

std::string gFileName("test");

#define PREPARE_PARSER(str) \
    SymbolTable table; \
    std::istringstream stream(str); \
    std::istreambuf_iterator<char> first(stream); \
    std::istreambuf_iterator<char> last; \
    ObjectList objlist; \
    Scanner scanner(first, last, gFileName, &table); \
    Parser parser(&scanner, &table, &objlist);

template<typename T>
void ExpectObjectEq(const T& expect, const Object* obj) {
    const T* p = dynamic_cast<const T*>(obj);
    EXPECT_NE(nullptr, p);
    if (p != nullptr) {
        EXPECT_EQ(expect, *p);
    }
}

TEST(Parser, Integer) {
    PREPARE_PARSER("42")
    Object* obj = parser.Parse();
    ExpectObjectEq<IntegerObject>(IntegerObject(42), obj);
}

TEST(Parser, List1) {
    PREPARE_PARSER("(x y z #t #f)");
    Object* obj = parser.Parse();
    EXPECT_EQ("(x y z #t #f)", obj->ToString());
}

TEST(Parser, List2) {
    PREPARE_PARSER("(x y z #t . #f)");
    Object* obj = parser.Parse();
    EXPECT_EQ("(x y z #t . #f)", obj->ToString());
}

TEST(Parser, List3) {
    PREPARE_PARSER("()");
    Object* obj = parser.Parse();
    EXPECT_EQ("()", obj->ToString());
}

TEST(Parser, Vector) {
    PREPARE_PARSER("#(#\\x #\\y)")
    Object* obj = parser.Parse();
    EXPECT_EQ("#(#\\x #\\y)", obj->ToString());
}

TEST(Parser, Define) {
    PREPARE_PARSER("(define (foo x y) (+ (* x 2) y))")
    Object* obj = parser.Parse();
    EXPECT_EQ("(define (foo x y) (+ (* x 2) y))", obj->ToString());
}

TEST(Parser, String) {
    PREPARE_PARSER("(print \"Hello, World!\")")
    Object* obj = parser.Parse();
    EXPECT_EQ("(print \"Hello, World!\")", obj->ToString());
}

TEST(Parser, Quote) {
    PREPARE_PARSER("'(foo x y)")
    Object* obj = parser.Parse();
    EXPECT_EQ("(quote (foo x y))", obj->ToString());
}
