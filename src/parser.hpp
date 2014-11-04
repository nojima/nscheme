#pragma once

#include <unordered_map>
#include "allocator.hpp"
#include "node.hpp"
#include "object.hpp"
#include "source_map.hpp"
#include "symbol_table.hpp"


namespace nscheme {


class Parser {
public:
    Parser(SymbolTable* symbol_table, SourceMap* source_map,
           std::unordered_map<Symbol, Value>& global_variables)
        : symbol_table_(symbol_table)
        , source_map_(source_map)
        , global_variables_(global_variables)
        , kwd_lambda_(symbol_table->intern("lambda"))
        , kwd_if_(symbol_table->intern("if"))
        , kwd_set_bang_(symbol_table->intern("set!"))
        , kwd_define_(symbol_table->intern("define"))
        , kwd_quote_(symbol_table->intern("quote")) {}

    std::unique_ptr<Node> parse(Value datum);

private:

    struct LocalNames {
        explicit LocalNames(LocalNames* parent): parent(parent) {}
        LocalNames* parent;
        std::unordered_map<Symbol, size_t> name2index;
    };

    bool lookupSymbol(Symbol symbol, const LocalNames& names, std::pair<size_t, size_t>* out) const;
    std::unique_ptr<Node> parseExprOrDefine(Value value, const Position& position, LocalNames& names);
    std::unique_ptr<ExprNode> parseExpr(Value value, const Position& position, LocalNames& names);
    std::unique_ptr<ExprNode> parseVariable(Symbol symbol, const Position& position, LocalNames& names);
    std::unique_ptr<ExprNode> parseProcedureCall(PairObject* list, const Position& position, LocalNames& names);
    std::unique_ptr<ExprNode> parseLambda(Value value, const Position& position, LocalNames& names);
    std::unique_ptr<ExprNode> parseIf(Value value, const Position& position, LocalNames& names);
    std::unique_ptr<ExprNode> parseAssignment(Value value, const Position& position, LocalNames& names);
    std::unique_ptr<ExprNode> parseQuote(Value value, const Position& position, LocalNames& names);
    std::unique_ptr<Node> parseDefine(Value value, const Position& position, LocalNames& names);

    SymbolTable* symbol_table_;
    SourceMap* source_map_;
    std::unordered_map<Symbol, Value>& global_variables_;

    Symbol kwd_lambda_;
    Symbol kwd_if_;
    Symbol kwd_set_bang_;
    Symbol kwd_define_;
    Symbol kwd_quote_;
};


struct ParseError: std::runtime_error {
    ParseError(const Position& position, const std::string& message)
        : std::runtime_error(position.toString() + ": " + message) {}
};


}   // namespace nscheme
