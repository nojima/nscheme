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
            throw std::runtime_error("invalid token type: expected Identifier, actually " + toString());
        return identifier_;
    }

    bool Boolean() const {
        if (type_ == TokenType::kFalse)
            return false;
        if (type_ == TokenType::kTrue)
            return true;
        throw std::runtime_error("invalid token type: expected Boolean, actually " + toString());
    }

    std::int64_t Integer() const {
        if (type_ != TokenType::kInteger)
            throw std::runtime_error("invalid tokne type: expected Integer, actually " + toString());
        return integer_;
    }

    double Float() const {
        if (type_ != TokenType::kFloat)
            throw std::runtime_error("invalid token type: expected Float, actually " + toString());
        return float_;
    }

    char Character() const {
        if (type_ != TokenType::kCharacter)
            throw std::runtime_error("invalid token type: expected Character, actually " + toString());
        return character_;
    }

    const std::string& String() const {
        if (type_ != TokenType::kString)
            throw std::runtime_error("invalid token type: expected String, actually " + toString());
        return string_;
    }

    bool operator==(const Token& rhs) const noexcept;

    bool operator!=(const Token& rhs) const noexcept {
        return !(*this == rhs);
    }

    std::string toString() const;

private:
    TokenType type_;
    Symbol identifier_;
    std::int64_t integer_;
    double float_;
    char character_;
    std::string string_;
};

}   // namespace nscheme
