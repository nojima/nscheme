#pragma once

#include <stdexcept>
#include <string>

#include "symbol.hpp"

namespace nscheme {

enum class TokenType {
    kEof,
    kIdentifier,
    kTrue,
    kFalse,
    kInteger,
    kFloat,
    kCharacter,
    kString,
    kOpenParen,
    kCloseParen,
    kOpenVector,
    kOpenByteVector,
    kQuote,
    kBackQuote,
    kComma,
    kCommaAt,
    kPeriod,
};

class Token {
public:
    TokenType Type() const noexcept {
        return type_;
    }

    Symbol Identifier() const {
        if (type_ != TokenType::kIdentifier)
            throw std::runtime_error("invalid token type: expected Identifier, actually " + ToString());
        return identifier_;
    }

    bool Boolean() const {
        if (type_ == TokenType::kFalse)
            return false;
        if (type_ == TokenType::kTrue)
            return true;
        throw std::runtime_error("invalid token type: expected Boolean, actually " + ToString());
    }

    std::int64_t Integer() const {
        if (type_ != TokenType::kInteger)
            throw std::runtime_error("invalid tokne type: expected Integer, actually " + ToString());
        return integer_;
    }

    double Float() const {
        if (type_ != TokenType::kFloat)
            throw std::runtime_error("invalid token type: expected Float, actually " + ToString());
        return float_;
    }

    char Character() const {
        if (type_ != TokenType::kCharacter)
            throw std::runtime_error("invalid token type: expected Character, actually " + ToString());
        return character_;
    }

    const std::string& String() const {
        if (type_ != TokenType::kString)
            throw std::runtime_error("invalid token type: expected String, actually " + ToString());
        return string_;
    }

    bool operator==(const Token& rhs) const noexcept;

    bool operator!=(const Token& rhs) const noexcept {
        return !(*this == rhs);
    }

    std::string ToString() const;

    static Token CreateEof() {
        return Token(TokenType::kEof);
    }

    static Token CreateIdentifier(const std::string& name, SymbolTable* table) {
        Token t(TokenType::kIdentifier);
        t.identifier_ = table->Get(name);
        return t;
    }

    static Token CreateBoolean(bool b) {
        if (b)
            return Token(TokenType::kTrue);
        else
            return Token(TokenType::kFalse);
    }

    static Token CreateInteger(std::int64_t n) {
        Token t(TokenType::kInteger);
        t.integer_ = n;
        return t;
    }

    static Token CreateFloat(double x) {
        Token t(TokenType::kFloat);
        t.float_ = x;
        return t;
    }

    static Token CreateString(const std::string& str) {
        Token t(TokenType::kString);
        t.string_ = str;
        return t;
    }

    static Token CreateOpenParen() {
        return Token(TokenType::kOpenParen);
    }

    static Token CreateCloseParen() {
        return Token(TokenType::kCloseParen);
    }

    static Token CreateOpenVector() {
        return Token(TokenType::kOpenVector);
    }

    static Token CreateOpenByteVector() {
        return Token(TokenType::kOpenByteVector);
    }

    static Token CreateQuote() {
        return Token(TokenType::kQuote);
    }

    static Token CreateBackQuote() {
        return Token(TokenType::kBackQuote);
    }

    static Token CreateComma() {
        return Token(TokenType::kComma);
    }

    static Token CreateCommaAt() {
        return Token(TokenType::kCommaAt);
    }

    static Token CreatePeriod() {
        return Token(TokenType::kPeriod);
    }

private:
    explicit Token(TokenType type): type_(type), identifier_(nullptr) {}

    TokenType type_ = TokenType::kEof;
    Symbol identifier_;
    std::int64_t integer_ = 0;
    double float_ = 0.0;
    char character_ = '\0';
    std::string string_;
};

}   // namespace nscheme
