#include "token.hpp"

namespace nscheme {

std::string Token::toString() const {
    switch (type_) {
    case TokenType::kEof:
        return "EOF";
    case TokenType::kIdentifier:
        return "[Identifier " + identifier_.toString() + "]";
    case TokenType::kTrue:
        return "[Boolean true]";
    case TokenType::kFalse:
        return "[Boolean false]";
    case TokenType::kInteger:
        return "[Number " + std::to_string(integer_) + "]";
    case TokenType::kFloat:
        return "[Number " + std::to_string(float_) + "]";
    case TokenType::kCharacter:
        return "[Character '" + std::string(1, character_) + "']";
    case TokenType::kString:
        return "[String '" + string_ + "]";
    case TokenType::kOpenParen:
        return "(";
    case TokenType::kCloseParen:
        return ")";
    case TokenType::kOpenVector:
        return "#(";
    case TokenType::kOpenByteVector:
        return "#u8(";
    case TokenType::kQuote:
        return "'";
    case TokenType::kBackQuote:
        return "`";
    case TokenType::kComma:
        return ",";
    case TokenType::kCommaAt:
        return ",@";
    case TokenType::kPeriod:
        return ".";
    default:
        return "UnknownToken";
    }
}

bool Token::operator==(const Token& rhs) const noexcept {
    if (type_ != rhs.type_)
        return false;
    switch (type_) {
    case TokenType::kIdentifier:
        return identifier_ == rhs.identifier_;
    case TokenType::kInteger:
        return integer_ == rhs.integer_;
    case TokenType::kFloat:
        return float_ == rhs.float_;
    case TokenType::kCharacter:
        return character_ == rhs.character_;
    case TokenType::kString:
        return string_ == rhs.string_;
    default:
        return true;
    }
}

}   // namespace nscheme
