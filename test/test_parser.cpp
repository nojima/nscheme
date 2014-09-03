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

TEST(Parser, Integer) {
    PREPARE_PARSER("42")
    ObjectRef obj = parser.Parse();
    EXPECT_EQ("42", ObjectToString(obj));
}

TEST(Parser, List1) {
    PREPARE_PARSER("(x y z #t #f)");
    ObjectRef obj = parser.Parse();
    EXPECT_EQ("(x y z #t #f)", ObjectToString(obj));
}

TEST(Parser, List2) {
    PREPARE_PARSER("(x y z #t . #f)");
    ObjectRef obj = parser.Parse();
    EXPECT_EQ("(x y z #t . #f)", ObjectToString(obj));
}

TEST(Parser, List3) {
    PREPARE_PARSER("()");
    ObjectRef obj = parser.Parse();
    EXPECT_EQ("()", ObjectToString(obj));
}

TEST(Parser, Vector) {
    PREPARE_PARSER("#(#\\x #\\y)")
    ObjectRef obj = parser.Parse();
    EXPECT_EQ("#(#\\x #\\y)", ObjectToString(obj));
}

TEST(Parser, Define) {
    PREPARE_PARSER("(define (foo x y) (+ (* x 2) y))")
    ObjectRef obj = parser.Parse();
    EXPECT_EQ("(define (foo x y) (+ (* x 2) y))", ObjectToString(obj));
}

TEST(Parser, String) {
    PREPARE_PARSER("(print \"Hello, World!\")")
    ObjectRef obj = parser.Parse();
    EXPECT_EQ("(print \"Hello, World!\")", ObjectToString(obj));
}

TEST(Parser, Quote) {
    PREPARE_PARSER("'(foo x y)")
    ObjectRef obj = parser.Parse();
    EXPECT_EQ("(quote (foo x y))", ObjectToString(obj));
}
