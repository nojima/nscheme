#include "scanner.hpp"

#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

using namespace nscheme;

std::string gFileName("test");

#define PREPARE_SCANNER(str) \
    SymbolTable table; \
    std::istringstream stream(str); \
    std::istreambuf_iterator<char> first(stream); \
    std::istreambuf_iterator<char> last; \
    Scanner scanner(first, last, gFileName, &table); \
    scanner.Next();

TEST(Scanner, Integer1) {
    PREPARE_SCANNER("42")
    EXPECT_EQ(Token::CreateInteger(42), scanner.Get());
}

TEST(Scanner, Integer2) {
    PREPARE_SCANNER("+1")
    EXPECT_EQ(Token::CreateInteger(1), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, Integer3) {
    PREPARE_SCANNER("-1")
    EXPECT_EQ(Token::CreateInteger(-1), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, Identifier1) {
    PREPARE_SCANNER("hoge")
    EXPECT_EQ(Token::CreateIdentifier("hoge", &table), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, Identifier2) {
    PREPARE_SCANNER("|hello world\\t\\|foo\\x40;hoge.com\\n|")
    EXPECT_EQ(Token::CreateIdentifier("hello world\t|foo@hoge.com\n", &table), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, Identifier3) {
    PREPARE_SCANNER("+")
    EXPECT_EQ(Token::CreateIdentifier("+", &table), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, Identifier4) {
    PREPARE_SCANNER("+x")
    EXPECT_EQ(Token::CreateIdentifier("+x", &table), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, Identifier5) {
    PREPARE_SCANNER("+.x")
    EXPECT_EQ(Token::CreateIdentifier("+.x", &table), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, Identifier6) {
    PREPARE_SCANNER(".x")
    EXPECT_EQ(Token::CreateIdentifier(".x", &table), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, Boolean) {
    PREPARE_SCANNER("#t #f")
    EXPECT_EQ(Token::CreateBoolean(true), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateBoolean(false), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, Vector) {
    PREPARE_SCANNER("#( #u8(")
    EXPECT_EQ(Token::CreateOpenVector(), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateOpenByteVector(), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, Label) {
    PREPARE_SCANNER("#10")
    EXPECT_EQ(Token::CreateLabel(10), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, Characters) {
    PREPARE_SCANNER("#\\a #\\x #\\space #\\x40 #\\ ")
    std::vector<Token> expected_tokens = {
        Token::CreateCharacter('a'),
        Token::CreateCharacter('x'),
        Token::CreateCharacter(' '),
        Token::CreateCharacter('@'),
        Token::CreateCharacter(' '),
        Token::CreateEof(),
    };
    for (const auto& expected : expected_tokens) {
        EXPECT_EQ(expected, scanner.Get());
        scanner.Next();
    }
}

TEST(Scanner, Comment) {
    PREPARE_SCANNER("10 ; this is a comment!\n!")
    std::vector<Token> expected_tokens = {
        Token::CreateInteger(10),
        Token::CreateIdentifier("!", &table),
        Token::CreateEof(),
    };
    for (const auto& expected : expected_tokens) {
        EXPECT_EQ(expected, scanner.Get());
        scanner.Next();
    }
}

TEST(Scanner, String1) {
    PREPARE_SCANNER("\"hello, world\"")
    EXPECT_EQ(Token::CreateString("hello, world"), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, String2) {
    PREPARE_SCANNER("\"hello\\tworld\"")
    EXPECT_EQ(Token::CreateString("hello\tworld"), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, String3) {
    PREPARE_SCANNER("\"hello\\x40;world\"")
    EXPECT_EQ(Token::CreateString("hello@world"), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, String4) {
    PREPARE_SCANNER("\"hello\\\"world\"")
    EXPECT_EQ(Token::CreateString("hello\"world"), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, String5) {
    PREPARE_SCANNER("\"hello\\\\world\"")
    EXPECT_EQ(Token::CreateString("hello\\world"), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, String6) {
    PREPARE_SCANNER("\"hello \\  \n\tworld\"")
    EXPECT_EQ(Token::CreateString("hello world"), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(), scanner.Get());
}

TEST(Scanner, DefineExpression) {
    PREPARE_SCANNER("(define (foo . x) (bar x #()))")
    std::vector<Token> expected_tokens = {
        Token::CreateOpenParen(),
        Token::CreateIdentifier("define", &table),
        Token::CreateOpenParen(),
        Token::CreateIdentifier("foo", &table),
        Token::CreatePeriod(),
        Token::CreateIdentifier("x", &table),
        Token::CreateCloseParen(),
        Token::CreateOpenParen(),
        Token::CreateIdentifier("bar", &table),
        Token::CreateIdentifier("x", &table),
        Token::CreateOpenVector(),
        Token::CreateCloseParen(),
        Token::CreateCloseParen(),
        Token::CreateCloseParen(),
        Token::CreateEof(),
    };
    for (const auto& expected : expected_tokens) {
        EXPECT_EQ(expected, scanner.Get());
        scanner.Next();
    }
}
