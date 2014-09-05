#pragma once

#include "allocator.hpp"
#include "node.hpp"
#include "object.hpp"
#include "source_map.hpp"
#include "symbol_table.hpp"

namespace nscheme {

class Analyzer {
public:
    Analyzer(SymbolTable* symbol_table, Allocator* allocator,
             SourceMap* source_map)
        : symbol_table_(symbol_table), allocator_(allocator)
        , source_map_(source_map)
        , kwd_lambda_(symbol_table->intern("lambda"))
        , kwd_if_(symbol_table->intern("if"))
        , kwd_set_bang_(symbol_table->intern("set!"))
        , kwd_define_(symbol_table->intern("define"))
        , kwd_quote_(symbol_table->intern("quote")) {}

    Node* analyze(Value datum);

private:
    Node* analyzeExprOrDefine(Value value, const Position& position);
    ExprNode* analyzeExpr(Value value, const Position& position);
    ExprNode* analyzeProcedureCall(PairObject* list, const Position& position);
    ExprNode* analyzeLambda(Value value, const Position& position);
    ExprNode* analyzeIf(Value value, const Position& position);
    ExprNode* analyzeAssignment(Value value, const Position& position);
    ExprNode* analyzeQuote(Value value, const Position& position);
    Node* analyzeDefine(Value value, const Position& position);

    SymbolTable* symbol_table_;
    Allocator* allocator_;
    SourceMap* source_map_;

    Symbol kwd_lambda_;
    Symbol kwd_if_;
    Symbol kwd_set_bang_;
    Symbol kwd_define_;
    Symbol kwd_quote_;
};

struct AnalyzeError: std::runtime_error {
    AnalyzeError(const Position& position, const std::string& message)
        : std::runtime_error(position.toString() + ": " + message) {}
};

}
