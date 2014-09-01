#include "scanner.hpp"

#include <cctype>

namespace {

inline bool IsSpecialInitial(int ch) {
    switch (ch) {
    case '!': case '$': case '%': case '&': case '*': case '/': case ':':
    case '<': case '=': case '>': case '?': case '^': case '_': case '~':
        return true;
    default:
        return false;
    }
}

inline bool IsInitial(int ch) {
    return isalpha(ch) || IsSpecialInitial(ch);
}

inline bool IsExplicitSign(int ch) {
    return ch == '+' || ch == '-';
}

inline bool IsSpecialSubsequent(int ch) {
    return ch == '.' || ch == '@' || IsExplicitSign(ch);
}

inline bool IsSubsequent(int ch) {
    return IsInitial(ch) || isdigit(ch) || IsSpecialSubsequent(ch);
}

inline bool IsSignSubsequent(int ch) {
    return IsInitial(ch) || IsExplicitSign(ch) || ch == '@';
}

inline bool IsDotSubsequent(int ch) {
    return IsSignSubsequent(ch) || ch == '.';
}

}   // namespace

namespace nscheme {

void Scanner::Next() {
retry:
    int ch = CurrChar();
    while (std::isspace(ch))
        ch = NextChar();

    if (ch == -1) {
        token_ = Token::CreateEof();
        return;
    }
    if (isdigit(ch)) {
        // TODO: support float
        token_ = Token::CreateInteger(DecodeDigit());
        return;
    }
    if (IsInitial(ch)) {
        std::string buffer(1, ch);
        ch = NextChar();
        while (IsSubsequent(ch)) {
            buffer.push_back(ch);
            ch = NextChar();
        }
        token_ = Token::CreateIdentifier(buffer, table_);
        return;
    }
    if (IsExplicitSign(ch)) {
        TokenizeAfterExplicitSign();
        return;
    }
    if (ch == '.') {
        TokenizeAfterDot(-1);
        return;
    }
    if (ch == '|') {
        TokenizeEnclosedIdentifier();
        return;
    }
    if (ch == '(') {
        token_ = Token::CreateOpenParen();
        NextChar();
        return;
    }
    if (ch == ')') {
        token_ = Token::CreateCloseParen();
        NextChar();
        return;
    }
    if (ch == '\'') {
        token_ = Token::CreateQuote();
        NextChar();
        return;
    }
    if (ch == '`') {
        token_ = Token::CreateBackQuote();
        NextChar();
        return;
    }
    if (ch == ',') {
        const int ch2 = NextChar();
        if (ch2 == '@') {
            token_ = Token::CreateCommaAt();
            NextChar();
            return;
        }
        token_ = Token::CreateComma();
        return;
    }
    if (ch == '.') {
        token_ = Token::CreatePeriod();
        NextChar();
        return;
    }
    if (ch == '#') {
        bool comment = TokenizeAfterSharp();
        if (comment)
            goto retry;
        return;
    }
    if (ch == ';') {
        while (ch != -1 && ch != '\n')
            ch = NextChar();
        goto retry;
    }
}

void Scanner::TokenizeAfterExplicitSign() {
    const int first_ch = CurrChar();
    int ch = NextChar();
    if (isdigit(ch)) {
        // number
        const std::int64_t n = (first_ch == '+') ? DecodeDigit() : -DecodeDigit();
        token_ = Token::CreateInteger(n);
        return;
    }
    // peculiar identifier
    if (IsSignSubsequent(ch)) {
        std::string buffer(1, first_ch);
        buffer.push_back(ch);
        ch = NextChar();
        while (IsSubsequent(ch)) {
            buffer.push_back(ch);
            ch = NextChar();
        }
        token_ = Token::CreateIdentifier(buffer, table_);
        return;
    }
    if (ch == '.') {
        TokenizeAfterDot(first_ch);
        return;
    }
    token_ = Token::CreateIdentifier(std::string(1, first_ch), table_);
}

void Scanner::TokenizeEnclosedIdentifier() {
    std::string buffer;
    int ch = NextChar();
    while (ch != -1 && ch != '|') {
        if (ch == '\\') {
            ch = NextChar();
            if (ch == 'x') {
                ch = NextChar();
                if (!isalnum(ch))
                    throw ScanError(CurrPos(), "expected hex");
                buffer.push_back(DecodeHex());
                if (CurrChar() != ';')
                    throw ScanError(CurrPos(), "expected ';'");
                ch = NextChar();
            } else if (ch == '|') {
                buffer.push_back('|');
                ch = NextChar();
            } else if (isalpha(ch)) {
                buffer.push_back(DecodeMnemoicEscape());
                ch = NextChar();
            } else {
                throw ScanError(CurrPos(), "unknown escape");
            }
        } else {
            buffer.push_back(ch);
            ch = NextChar();
        }
    }
    if (ch != '|')
        throw ScanError(CurrPos(), "expected '|'");
    NextChar();
    token_ = Token::CreateIdentifier(buffer, table_);
}

bool Scanner::TokenizeAfterSharp() {
    int ch = NextChar();
    if (ch == '(') {
        token_ = Token::CreateOpenVector();
        NextChar();
        return false;
    }
    if (ch == '\\') {
        TokenizeCharacter();
        return false;
    }
    if (ch == 't') {
        token_ = Token::CreateBoolean(true);
        NextChar();
        return false;
    }
    if (ch == 'f') {
        token_ = Token::CreateBoolean(false);
        NextChar();
        return false;
    }
    if (ch == 'u') {
        ch = NextChar();
        if (ch != '8')
            throw ScanError(CurrPos(), "expected '8'");
        ch = NextChar();
        if (ch != '(')
            throw ScanError(CurrPos(), "expected '('");
        token_ = Token::CreateOpenByteVector();
        NextChar();
        return false;
    }
    if (isdigit(ch)) {
        std::string buffer;
        while (isdigit(ch)) {
            buffer.push_back(ch);
            ch = NextChar();
        }
        token_ = Token::CreateLabel(std::stoll(buffer));
        return false;
    }
    token_ = Token::CreateSharp();
    return false;
}

void Scanner::TokenizeCharacter() {
    int ch = NextChar();
    if (ch == 'x') {
        ch = NextChar();
        if (isalnum(ch)) {
            // #\x <hex scalar value>
            token_ = Token::CreateCharacter(DecodeHex());
            return;
        }
        // character 'x'
        token_ = Token::CreateCharacter('x');
        return;
    }

    int ch2 = NextChar();
    if (isalpha(ch) && isalpha(ch2)) {
        // #\ <character name>
        std::string buffer(1, ch);
        while (isalpha(ch2)) {
            buffer.push_back(ch2);
            ch2 = NextChar();
        }
        token_ = CharacterNameToToken(buffer);
        return;
    }

    // #\ <character>
    token_ = Token::CreateCharacter(ch);
    return;
}

void Scanner::TokenizeAfterDot(int first_ch) {
    int ch = NextChar();
    if (isdigit(ch)) {
        // TODO: support float
        throw ScanError(CurrPos(), "not implemented");
    }
    if (IsDotSubsequent(ch)) {
        std::string buffer;
        if (first_ch != -1)
            buffer.push_back(first_ch);
        buffer.push_back('.');
        buffer.push_back(ch);
        ch = NextChar();
        while (IsSubsequent(ch)) {
            buffer.push_back(ch);
            ch = NextChar();
        }
        token_ = Token::CreateIdentifier(buffer, table_);
        return;
    }
    if (first_ch == -1) {
        token_ = Token::CreatePeriod();
        return;
    }
    throw ScanError(CurrPos(), "expected a digit or dot subsequent");
}

Token Scanner::CharacterNameToToken(const std::string& name) {
    if (name == "alarm")
        return Token::CreateCharacter(0x07);
    if (name == "backspace")
        return Token::CreateCharacter(0x08);
    if (name == "delete")
        return Token::CreateCharacter(0x7F);
    if (name == "escape")
        return Token::CreateCharacter(0x1B);
    if (name == "newline")
        return Token::CreateCharacter(0x0A);
    if (name == "null")
        return Token::CreateCharacter(0x00);
    if (name == "return")
        return Token::CreateCharacter(0x0D);
    if (name == "space")
        return Token::CreateCharacter(0x20);
    if (name == "tab")
        return Token::CreateCharacter(0x09);
    throw ScanError(CurrPos(), "unknown character name: " + name);
}

char Scanner::DecodeMnemoicEscape() {
    switch (CurrChar()) {
    case 'a': return '\a';
    case 'b': return '\b';
    case 't': return '\t';
    case 'n': return '\n';
    case 'r': return '\r';
    default:
        throw ScanError(CurrPos(), "unknown mnemoic escape: " + std::string(1, CurrChar()));
    }
}

std::int64_t Scanner::DecodeDigit() {
    std::string buffer;
    int ch = CurrChar();
    while (isdigit(ch)) {
        buffer.push_back(ch);
        ch = NextChar();
    }
    return std::stoll(buffer);
}

char Scanner::DecodeHex() {
    std::string buffer;
    int ch = CurrChar();
    while (isalnum(ch)) {
        buffer.push_back(ch);
        ch = NextChar();
    }
    return std::stoi(buffer, 0, 16);
}

}   // namespace nscheme
