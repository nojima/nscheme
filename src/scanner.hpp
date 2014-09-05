#pragma once

#include <stdexcept>
#include "reader.hpp"
#include "symbol_table.hpp"
#include "token.hpp"

namespace nscheme {

class Scanner {
public:
    Scanner(Reader* reader, SymbolTable* symbol_table)
        : reader_(reader), symbol_table_(symbol_table)
        , ch_(reader->getChar()) {}

    Token getToken();

private:
    Position getPosition() {
        return reader_->getPosition();
    }

    Token tokenizeIdentifier();
    Token tokenizeCharacter();
    Token tokenizeString();
    Token tokenizeNumber(bool negative);

    Reader* reader_;
    SymbolTable* symbol_table_;
    int ch_;
};

struct ScanError: std::runtime_error {
    ScanError(const Position& position, const std::string& message)
        : std::runtime_error(position.toString() + ": " + message) {}
};

}
