#include "scanner.hpp"
#include <cctype>
#include <string>

namespace {

inline bool isSpecialInitial(int ch) {
    switch (ch) {
    case '!': case '$': case '%': case '&': case '*': case '/': case ':':
    case '<': case '=': case '>': case '?': case '^': case '_': case '~':
        return true;
    default:
        return false;
    }
}

inline bool isInitial(int ch) {
    return isalpha(ch) || isSpecialInitial(ch);
}

inline bool isSpecialSubsequent(int ch) {
    switch (ch) {
    case '+': case '-': case '.': case '@':
        return true;
    default:
        return false;
    }
}

inline bool isSubsequent(int ch) {
    return isInitial(ch) || isdigit(ch) || isSpecialSubsequent(ch);
}

}

namespace nscheme {

Token Scanner::getToken() {
    while (isspace(ch_)) {
        ch_ = reader_->getChar();
    }

    if (isInitial(ch_)) {
        return tokenizeIdentifier();
    }

    if (isdigit(ch_)) {
        return tokenizeNumber(false);
    }

    if (ch_ == '#') {
        ch_ = reader_->getChar();
        if (ch_ == 't') {
            ch_ = reader_->getChar();
            return Token(TokenType::kTrue, getPosition());
        }
        if (ch_ == 'f') {
            ch_ = reader_->getChar();
            return Token(TokenType::kFalse, getPosition());
        }
        if (ch_ == '(') {
            ch_ = reader_->getChar();
            return Token(TokenType::kSharpOpenParen, getPosition());
        }
        if (ch_ == '\\') {
            return tokenizeCharacter();
        }
    }

    if (ch_ == '"') {
        return tokenizeString();
    }

    if (ch_ == '+' || ch_ == '-') {
        const char sign = ch_;
        ch_ = reader_->getChar();
        if (isdigit(ch_) || ch_ == '.') {
            return tokenizeNumber(sign == '-');
        } else {
            Symbol symbol = symbol_table_->intern(std::string(1, sign));
            return Token::makeIdentifier(getPosition(), symbol);
        }
    }

    if (ch_ == '.') {
        ch_ = reader_->getChar();
        if (isdigit(ch_)) {
            return tokenizeNumber(false);
        } else {
            std::string buffer(".");
            while (isSubsequent(ch_)) {
                buffer.push_back(ch_);
                ch_ = reader_->getChar();
            }
            Symbol symbol = symbol_table_->intern(buffer);
            return Token::makeIdentifier(getPosition(), symbol);
        }
    }

    if (ch_ == '(') {
        ch_ = reader_->getChar();
        return Token(TokenType::kOpenParen, getPosition());
    }

    if (ch_ == ')') {
        ch_ = reader_->getChar();
        return Token(TokenType::kCloseParen, getPosition());
    }

    if (ch_ == '\'') {
        ch_ = reader_->getChar();
        return Token(TokenType::kQuote, getPosition());
    }

    if (ch_ == '`') {
        ch_ = reader_->getChar();
        return Token(TokenType::kBackQuote, getPosition());
    }

    if (ch_ == ',') {
        ch_ = reader_->getChar();
        if (ch_ == '@') {
            ch_ = reader_->getChar();
            return Token(TokenType::kCommaAt, getPosition());
        } else {
            return Token(TokenType::kComma, getPosition());
        }
    }

    if (ch_ == '.') {
        ch_ = reader_->getChar();
        return Token(TokenType::kPeriod, getPosition());
    }

    throw ScanError(getPosition(), "unexpected character: " + std::string(1, ch_));
}

Token Scanner::tokenizeIdentifier() {
    std::string buffer(1, ch_);
    ch_ = reader_->getChar();
    while (isSubsequent(ch_)) {
        buffer.push_back(ch_);
        ch_ = reader_->getChar();
    }
    Symbol symbol = symbol_table_->intern(buffer);
    return Token::makeIdentifier(getPosition(), symbol);
}

Token Scanner::tokenizeCharacter() {
    const int ch1 = reader_->getChar();
    const int ch2 = reader_->getChar();
    if (isalpha(ch1) && isalpha(ch2)) {
        std::string buffer;
        buffer.push_back(ch1);
        buffer.push_back(ch2);
        ch_ = reader_->getChar();
        while (isalpha(ch_))
            ch_ = reader_->getChar();
        uint32_t character;
        if (buffer == "space")
            character = ' ';
        else if (buffer == "newline")
            character = '\n';
        else
            throw ScanError(getPosition(), "unknown character name: " + buffer);
        return Token::makeCharacter(getPosition(), character);
    } else {
        ch_ = ch2;
        return Token::makeCharacter(getPosition(), ch1);
    }
}

Token Scanner::tokenizeString() {
    ch_ = reader_->getChar();
    std::string buffer;
    while (ch_ != EOF && ch_ != '"') {
        if (ch_ == '\\') {
            ch_ = reader_->getChar();
            switch (ch_) {
            case '"': buffer.push_back('"');
            case '\\': buffer.push_back('\\');
            case 'n': buffer.push_back('\n');
            case 'r': buffer.push_back('\r');
            case 't': buffer.push_back('\t');
            default: throw ScanError(getPosition(), "unknown escape: \\" + std::string(1, ch_));
            }
        } else {
            buffer.push_back(ch_);
        }
        ch_ = reader_->getChar();
    }
    if (ch_ != '"')
        throw ScanError(getPosition(), "unclosed string literal");
    ch_ = reader_->getChar();
    return Token::makeString(getPosition(), buffer);
}

Token Scanner::tokenizeNumber(bool negative) {
    std::string buffer;
    size_t n_period = 0;
    while (isdigit(ch_) || ch_ == '.') {
        buffer.push_back(ch_);
        if (ch_ == '.')
            ++n_period;
        ch_ = reader_->getChar();
    }
    if (n_period > 1)
        throw ScanError(getPosition(), "too many period in a number");
    const int sign = negative ? -1 : 1;
    if (n_period == 1)
        return Token::makeReal(getPosition(), sign * std::stod(buffer));
    else
        return Token::makeInteger(getPosition(), sign * std::stoll(buffer));
}

}
