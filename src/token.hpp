#pragma once

#include <stdexcept>
#include <string>

#include "position.hpp"
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
    kSharp,
    kEqual,
    kLabel,
};

class Token {
public:
    TokenType Type() const noexcept {
        return type_;
    }

    const Position& Pos() const noexcept {
        return position_;
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

    std::int64_t Label() const {
        if (type_ != TokenType::kLabel)
            throw std::runtime_error("invalid token type: expected Label, actually " + ToString());
        return integer_;
    }

    bool operator==(const Token& rhs) const noexcept;

    bool operator!=(const Token& rhs) const noexcept {
        return !(*this == rhs);
    }

    std::string ToString() const;

    static Token CreateEof(const Position& pos) {
        return Token(TokenType::kEof, pos);
    }

    static Token CreateIdentifier(const std::string& name, SymbolTable* table, const Position& pos) {
        Token t(TokenType::kIdentifier, pos);
        t.identifier_ = table->Get(name);
        return t;
    }

    static Token CreateBoolean(bool b, const Position& pos) {
        if (b)
            return Token(TokenType::kTrue, pos);
        else
            return Token(TokenType::kFalse, pos);
    }

    static Token CreateInteger(std::int64_t n, const Position& pos) {
        Token t(TokenType::kInteger, pos);
        t.integer_ = n;
        return t;
    }

    static Token CreateFloat(double x, const Position& pos) {
        Token t(TokenType::kFloat, pos);
        t.float_ = x;
        return t;
    }

    static Token CreateCharacter(char ch, const Position& pos) {
        Token t(TokenType::kCharacter, pos);
        t.character_ = ch;
        return t;
    }

    static Token CreateString(const std::string& str, const Position& pos) {
        Token t(TokenType::kString, pos);
        t.string_ = str;
        return t;
    }

    static Token CreateOpenParen(const Position& pos) {
        return Token(TokenType::kOpenParen, pos);
    }

    static Token CreateCloseParen(const Position& pos) {
        return Token(TokenType::kCloseParen, pos);
    }

    static Token CreateOpenVector(const Position& pos) {
        return Token(TokenType::kOpenVector, pos);
    }

    static Token CreateOpenByteVector(const Position& pos) {
        return Token(TokenType::kOpenByteVector, pos);
    }

    static Token CreateQuote(const Position& pos) {
        return Token(TokenType::kQuote, pos);
    }

    static Token CreateBackQuote(const Position& pos) {
        return Token(TokenType::kBackQuote, pos);
    }

    static Token CreateComma(const Position& pos) {
        return Token(TokenType::kComma, pos);
    }

    static Token CreateCommaAt(const Position& pos) {
        return Token(TokenType::kCommaAt, pos);
    }

    static Token CreatePeriod(const Position& pos) {
        return Token(TokenType::kPeriod, pos);
    }

    static Token CreateSharp(const Position& pos) {
        return Token(TokenType::kSharp, pos);
    }

    static Token CreateEqual(const Position& pos) {
        return Token(TokenType::kEqual, pos);
    }

    static Token CreateLabel(std::int64_t no, const Position& pos) {
        Token t(TokenType::kLabel, pos);
        t.integer_ = no;
        return t;
    }

private:
    explicit Token(TokenType type, Position position):
        type_(type), position_(position), identifier_(nullptr) {}

    TokenType type_ = TokenType::kEof;
    Position position_;
    Symbol identifier_;
    std::int64_t integer_ = 0;
    double float_ = 0.0;
    char character_ = '\0';
    std::string string_;
};

}   // namespace nscheme
