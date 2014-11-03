#pragma once

#include "allocator.hpp"
#include "node.hpp"
#include "object.hpp"
#include "source_map.hpp"
#include "symbol_table.hpp"


namespace nscheme {


class Parser {
public:
    Parser(SymbolTable* symbol_table, Allocator* allocator,
           SourceMap* source_map, std::vector<Symbol>* global_names)
        : symbol_table_(symbol_table), allocator_(allocator)
        , source_map_(source_map)
        , global_names_(global_names)
        , kwd_lambda_(symbol_table->intern("lambda"))
        , kwd_if_(symbol_table->intern("if"))
        , kwd_set_bang_(symbol_table->intern("set!"))
        , kwd_define_(symbol_table->intern("define"))
        , kwd_quote_(symbol_table->intern("quote"))
        {}

    Node* parse(Value datum);

private:

    struct LocalNames {
        LocalNames* parent;
        std::unordered_map<Symbol, size_t> name2index;
    };

    Node* parseExprOrDefine(Value value, const Position& position, LocalNames& names);
    ExprNode* parseExpr(Value value, const Position& position, LocalNames& names);
    ExprNode* parseProcedureCall(PairObject* list, const Position& position, LocalNames& names);
    ExprNode* parseLambda(Value value, const Position& position, LocalNames& names);
    ExprNode* parseIf(Value value, const Position& position, LocalNames& names);
    ExprNode* parseAssignment(Value value, const Position& position, LocalNames& names);
    ExprNode* parseQuote(Value value, const Position& position, LocalNames& names);
    Node* parseDefine(Value value, const Position& position, LocalNames& names);

    SymbolTable* symbol_table_;
    Allocator* allocator_;
    SourceMap* source_map_;
    std::vector<Symbol>* global_names_;

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
