#pragma once

#include <vector>
#include "parser.hpp"
#include "value.hpp"


namespace nscheme {

class Allocator;
class SymbolTable;


class SyntaxRule {
public:
    SyntaxRule(Value pattern, Value tmpl, std::vector<Symbol>& literals, LocalNames& names,
               Allocator* allocator, SymbolTable* symbol_table)
        : pattern_(pattern)
        , template_(tmpl)
        , literals_(literals)
        , names_(names)
        , allocator_(allocator)
        , symbol_table_(symbol_table)
        , kwd_underscore_(symbol_table->intern("-"))
        , kwd_ellipsis_(symbol_table->intern("..."))
    {
    }

    bool verifyPattern() const;

    Value replace(Value expr);

    bool match(Value pattern, Value expr, std::unordered_map<Symbol, Value>& mapping) const;

private:
    Value pattern_;
    Value template_;
    std::vector<Symbol>& literals_;
    LocalNames& names_;
    Allocator* allocator_;
    SymbolTable* symbol_table_;
    Symbol kwd_underscore_;
    Symbol kwd_ellipsis_;
};


} // namespace nscheme
