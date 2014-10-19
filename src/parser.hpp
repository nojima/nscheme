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
             SourceMap* source_map)
        : symbol_table_(symbol_table), allocator_(allocator)
        , source_map_(source_map)
        , kwd_lambda_(symbol_table->intern("lambda"))
        , kwd_if_(symbol_table->intern("if"))
        , kwd_set_bang_(symbol_table->intern("set!"))
        , kwd_define_(symbol_table->intern("define"))
        , kwd_quote_(symbol_table->intern("quote")) {}

    Node* parse(Value datum);

private:
    Node* parseExprOrDefine(Value value, const Position& position);
    ExprNode* parseExpr(Value value, const Position& position);
    ExprNode* parseProcedureCall(PairObject* list, const Position& position);
    ExprNode* parseLambda(Value value, const Position& position);
    ExprNode* parseIf(Value value, const Position& position);
    ExprNode* parseAssignment(Value value, const Position& position);
    ExprNode* parseQuote(Value value, const Position& position);
    Node* parseDefine(Value value, const Position& position);

    SymbolTable* symbol_table_;
    Allocator* allocator_;
    SourceMap* source_map_;

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

}
