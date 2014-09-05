#pragma once

#include <stdexcept>
#include <unordered_map>
#include <vector>
#include "scanner.hpp"
#include "symbol_table.hpp"
#include "allocator.hpp"
#include "value.hpp"
#include "source_map.hpp"

namespace nscheme {

class Parser {
public:
    Parser(Scanner* scanner, SymbolTable* symbol_table,
           Allocator* allocator, SourceMap* source_map)
        : scanner_(scanner), symbol_table_(symbol_table)
        , allocator_(allocator), source_map_(source_map)
        , token_(scanner_->getToken()) {}

    Value parse();

private:
    Value parseDatum();
    Value parseList();
    Value parseVector();
    Value parseAbbr();

    Scanner* scanner_;
    SymbolTable* symbol_table_;
    Allocator* allocator_;
    SourceMap* source_map_;
    Token token_;
};

struct ParseError: std::runtime_error {
    ParseError(const Position& position, const std::string& message)
        : std::runtime_error(position.toString() + ": " + message) {}
};

}
