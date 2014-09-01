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

Position gPos("test", 1, 1);
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
    EXPECT_EQ(Token::CreateInteger(42, gPos), scanner.Get());
}

TEST(Scanner, Integer2) {
    PREPARE_SCANNER("+1")
    EXPECT_EQ(Token::CreateInteger(1, gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, Integer3) {
    PREPARE_SCANNER("-1")
    EXPECT_EQ(Token::CreateInteger(-1, gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, Identifier1) {
    PREPARE_SCANNER("hoge")
    EXPECT_EQ(Token::CreateIdentifier("hoge", &table, gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, Identifier2) {
    PREPARE_SCANNER("|hello world\\t\\|foo\\x40;hoge.com\\n|")
    EXPECT_EQ(Token::CreateIdentifier("hello world\t|foo@hoge.com\n", &table, gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, Identifier3) {
    PREPARE_SCANNER("+")
    EXPECT_EQ(Token::CreateIdentifier("+", &table, gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, Identifier4) {
    PREPARE_SCANNER("+x")
    EXPECT_EQ(Token::CreateIdentifier("+x", &table, gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, Identifier5) {
    PREPARE_SCANNER("+.x")
    EXPECT_EQ(Token::CreateIdentifier("+.x", &table, gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, Identifier6) {
    PREPARE_SCANNER(".x")
    EXPECT_EQ(Token::CreateIdentifier(".x", &table, gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, Boolean) {
    PREPARE_SCANNER("#t #f")
    EXPECT_EQ(Token::CreateBoolean(true, gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateBoolean(false, gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, Vector) {
    PREPARE_SCANNER("#( #u8(")
    EXPECT_EQ(Token::CreateOpenVector(gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateOpenByteVector(gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, Label) {
    PREPARE_SCANNER("#10")
    EXPECT_EQ(Token::CreateLabel(10, gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, Characters) {
    PREPARE_SCANNER("#\\a #\\x #\\space #\\x40 #\\ ")
    std::vector<Token> expected_tokens = {
        Token::CreateCharacter('a', gPos),
        Token::CreateCharacter('x', gPos),
        Token::CreateCharacter(' ', gPos),
        Token::CreateCharacter('@', gPos),
        Token::CreateCharacter(' ', gPos),
        Token::CreateEof(gPos),
    };
    for (const auto& expected : expected_tokens) {
        EXPECT_EQ(expected, scanner.Get());
        scanner.Next();
    }
}

TEST(Scanner, Comment) {
    PREPARE_SCANNER("10 ; this is a comment!\n!")
    std::vector<Token> expected_tokens = {
        Token::CreateInteger(10, gPos),
        Token::CreateIdentifier("!", &table, gPos),
        Token::CreateEof(gPos),
    };
    for (const auto& expected : expected_tokens) {
        EXPECT_EQ(expected, scanner.Get());
        scanner.Next();
    }
}

TEST(Scanner, String1) {
    PREPARE_SCANNER("\"hello, world\"")
    EXPECT_EQ(Token::CreateString("hello, world", gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, String2) {
    PREPARE_SCANNER("\"hello\\tworld\"")
    EXPECT_EQ(Token::CreateString("hello\tworld", gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, String3) {
    PREPARE_SCANNER("\"hello\\x40;world\"")
    EXPECT_EQ(Token::CreateString("hello@world", gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, String4) {
    PREPARE_SCANNER("\"hello\\\"world\"")
    EXPECT_EQ(Token::CreateString("hello\"world", gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, String5) {
    PREPARE_SCANNER("\"hello\\\\world\"")
    EXPECT_EQ(Token::CreateString("hello\\world", gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, String6) {
    PREPARE_SCANNER("\"hello \\  \n\tworld\"")
    EXPECT_EQ(Token::CreateString("hello world", gPos), scanner.Get());
    scanner.Next();
    EXPECT_EQ(Token::CreateEof(gPos), scanner.Get());
}

TEST(Scanner, DefineExpression) {
    PREPARE_SCANNER("(define (foo . x) (bar x #()))")
    std::vector<Token> expected_tokens = {
        Token::CreateOpenParen(gPos),
        Token::CreateIdentifier("define", &table, gPos),
        Token::CreateOpenParen(gPos),
        Token::CreateIdentifier("foo", &table, gPos),
        Token::CreatePeriod(gPos),
        Token::CreateIdentifier("x", &table, gPos),
        Token::CreateCloseParen(gPos),
        Token::CreateOpenParen(gPos),
        Token::CreateIdentifier("bar", &table, gPos),
        Token::CreateIdentifier("x", &table, gPos),
        Token::CreateOpenVector(gPos),
        Token::CreateCloseParen(gPos),
        Token::CreateCloseParen(gPos),
        Token::CreateCloseParen(gPos),
        Token::CreateEof(gPos),
    };
    for (const auto& expected : expected_tokens) {
        EXPECT_EQ(expected, scanner.Get());
        scanner.Next();
    }
}
