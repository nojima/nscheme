#pragma once

#include <iterator>
#include <string>

#include "error.hpp"
#include "position.hpp"
#include "token.hpp"

namespace nscheme {

class Scanner {
public:
    Scanner(const std::istreambuf_iterator<char>& first,
            const std::istreambuf_iterator<char>& last,
            const std::string& filename,
            SymbolTable* table):
        it_(first), last_(last), filename_(filename), table_(table), token_(Token::CreateEof(CurrPos())) {}

    const Token& Get() const noexcept {
        return token_;
    }

    void Next();

private:
    int CurrChar() const {
        return (it_ == last_) ? -1 : *it_;
    }

    int NextChar() {
        ++it_;
        const int ch = CurrChar();
        if (ch == '\n') {
            ++line_;
            column_ = 0;
        } else {
            ++column_;
        }
        return ch;
    }

    Position CurrPos() const {
        return Position(filename_, line_, column_);
    }

    Token CharacterNameToToken(const std::string& name);

    void TokenizeAfterExplicitSign();
    void TokenizeEnclosedIdentifier();
    bool TokenizeAfterSharp();
    void TokenizeCharacter();
    void TokenizeAfterDot(int first_ch);
    void TokenizeString();
    char DecodeMnemoicEscape();
    std::int64_t DecodeDigit();
    char DecodeHex();

    std::istreambuf_iterator<char> it_;
    std::istreambuf_iterator<char> last_;
    const std::string& filename_;
    size_t line_ = 1;
    size_t column_ = 1;
    SymbolTable* table_;
    Token token_;
};

class ScanError: public BasicError {
public:
    ScanError(const Position& position, const std::string& message):
        BasicError(position, message) {}
};

}   // namespace nscheme
