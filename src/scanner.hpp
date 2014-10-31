#pragma once

#include <stdexcept>
#include "stream.hpp"
#include "symbol_table.hpp"
#include "token.hpp"


namespace nscheme {


class Scanner {
public:
    Scanner(Stream* stream, SymbolTable* symbol_table)
        : stream_(stream), symbol_table_(symbol_table)
        , ch_(stream->getChar()) {}

    Token getToken();

private:
    Position getPosition() {
        return stream_->getPosition();
    }

    Token tokenizeIdentifier();
    Token tokenizeCharacter();
    Token tokenizeString();
    Token tokenizeNumber(bool negative);

    Stream* stream_;
    SymbolTable* symbol_table_;
    int ch_;
};


struct ScanError: public std::runtime_error {
    ScanError(const Position& position, const std::string& message)
        : std::runtime_error(position.toString() + ": " + message) {}
};


}   // namespace nscheme
