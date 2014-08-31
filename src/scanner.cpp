#include "scanner.hpp"

#include <cctype>

namespace nscheme {

void Scanner::Next() {
    int ch = CurrChar();
    for (;;) {
        while (std::isspace(ch))
            ch = NextChar();

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
        if (ch == '#') {
            ch = NextChar();
            if (ch == '(') {
                token_ = Token::CreateOpenVector();
                NextChar();
                return;
            }
            if (ch == '\\') {
                // character literal
                ch = NextChar();
                if (ch == 'x') {
                    int ch2 = NextChar();
                    if (isalnum(ch2)) {
                        // #\x <hex scalar value>
                        std::string buffer;
                        while (isalnum(ch2)) {
                            buffer.push_back(ch2);
                            ch2 = NextChar();
                        }
                        token_ = Token::CreateCharacter(std::stoi(buffer, 0, 16));
                        return;
                    }
                    // character 'x'
                    token_ = Token::CreateCharacter('x');
                    return;
                }

                int ch2 = NextChar();
                if (isalpha(ch) && isalpha(ch2)) {
                    // #\ <character name>
                    std::string buffer;
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
            if (ch == 't') {
                token_ = Token::CreateBoolean(true);
                NextChar();
                return;
            }
            if (ch == 'f') {
                token_ = Token::CreateBoolean(false);
                NextChar();
                return;
            }
            if (ch == 'u') {
                const int ch2 = NextChar();
                if (ch2 != '8')
                    throw ScanError(CurrPos(), "expected '8'");
                const int ch3 = NextChar();
                if (ch3 != '(')
                    throw ScanError(CurrPos(), "expected '('");
                token_ = Token::CreateOpenByteVector();
                NextChar();
                return;
            }
            if (isdigit(ch)) {
                std::string buffer;
                while (isdigit(ch)) {
                    buffer.push_back(ch);
                    ch = NextChar();
                }
                token_ = Token::CreateLabel(std::stoi(buffer));
                return;
            }
            token_ = Token::CreateSharp();
            return;
        }
    }
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

}   // namespace nscheme
