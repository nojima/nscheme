#pragma once

#include <string>
#include <utility>


namespace nscheme {


enum class TokenType {
    kEof,
    kIdentifier,
    kFalse,
    kTrue,
    kInteger,
    kReal,
    kCharacter,
    kString,
    kOpenParen,
    kCloseParen,
    kSharpOpenParen,
    kQuote,
    kBackQuote,
    kComma,
    kCommaAt,
    kPeriod,
};


class Token {
public:
    Token(TokenType type, Position position)
        : type_(type)
        , position_(position)
        , symbol_(nullptr)
    {
    }

    static Token makeIdentifier(const Position& position, Symbol symbol)
    {
        Token t(TokenType::kIdentifier, position);
        t.symbol_ = symbol;
        return t;
    }

    static Token makeInteger(const Position& position, int64_t integer)
    {
        Token t(TokenType::kInteger, position);
        t.integer_ = integer;
        return t;
    }

    static Token makeReal(const Position& position, double real)
    {
        Token t(TokenType::kReal, position);
        t.real_ = real;
        return t;
    }

    static Token makeCharacter(const Position& position, uint32_t character)
    {
        Token t(TokenType::kCharacter, position);
        t.character_ = character;
        return t;
    }

    static Token makeString(const Position& position, const std::string& string)
    {
        Token t(TokenType::kString, position);
        t.string_ = string;
        return t;
    }

    TokenType getType() const noexcept { return type_; }

    const Position& getPosition() const noexcept { return position_; }

    Symbol getSymbol() const noexcept { return symbol_; }

    int64_t getInteger() const noexcept { return integer_; }

    double getReal() const noexcept { return real_; }

    uint32_t getCharacter() const noexcept { return character_; }

    const std::string& getString() const noexcept { return string_; }

private:
    TokenType type_;
    Position position_;
    Symbol symbol_;
    int64_t integer_ = 0;
    double real_ = 0.0;
    uint32_t character_ = '\0';
    std::string string_;
};


} // namespace nscheme
