#include "analyzer.hpp"

namespace {

using namespace nscheme;

bool isSelfEvaluating(Value value) {
    if (value.isInteger()) return true;
    if (value.isCharacter()) return true;
    if (value == Value::True) return true;
    if (value == Value::False) return true;
    if (value.isPointer()) {
        auto p1 = dynamic_cast<StringObject*>(value.asPointer());
        if (p1 != nullptr)
            return true;
        auto p2 = dynamic_cast<RealObject*>(value.asPointer());
        if (p2 != nullptr)
            return true;
        auto p3 = dynamic_cast<VectorObject*>(value.asPointer());
        if (p3 != nullptr)
            return true;
    }
    return false;
}

inline bool isPair(Value value) {
    if (!value.isPointer())
        return false;
    return dynamic_cast<PairObject*>(value.asPointer());
}

}

namespace nscheme {

Node* Analyzer::analyze(Value datum) {
    Position dummy(symbol_table_->intern(""), 1, 1);
    return analyzeExpr(datum, dummy);
}

ExprNode* Analyzer::analyzeExpr(Value value, const Position& position) {
    if (value.isSymbol()) {
        return allocator_->make<VariableNode>(position, value.asSymbol());
    }
    if (isSelfEvaluating(value)) {
        return allocator_->make<LiteralNode>(position, value);
    }

    if (!value.isPointer())
        throw AnalyzeError(position, "invalid expression");
    PairObject* p = dynamic_cast<PairObject*>(value.asPointer());
    if (p == nullptr)
        throw AnalyzeError(position, "invalid expression");

    Value head = p->getCar();
    /*
    if (head == Value::fromSymbol(kwd_lambda_)) {
        return analyzeLambda(p->getCdr(), (*source_map_)[p]);
    }
    */
    if (head == Value::fromSymbol(kwd_if_)) {
        return analyzeIf(p->getCdr(), source_map_->at(p));
    }
    if (head == Value::fromSymbol(kwd_set_bang_)) {
        return analyzeAssignment(p->getCdr(), source_map_->at(p));
    }
    if (head == Value::fromSymbol(kwd_quote_)) {
        return analyzeQuote(p->getCdr(), source_map_->at(p));
    }
    return analyzeProcedureCall(p, source_map_->at(p));
}

ExprNode* Analyzer::analyzeProcedureCall(PairObject* list, const Position& position) {
    ExprNode* callee = analyzeExpr(list->getCar(), position);
    std::vector<ExprNode*> args;
    Value v = list->getCdr();
    while (v != Value::Nil) {
        if (!isPair(v))
            throw AnalyzeError(position, "invalid procedure call");
        PairObject* p = static_cast<PairObject*>(v.asPointer());
        args.push_back(analyzeExpr(p->getCar(), source_map_->at(p)));
        v = p->getCdr();
    }
    return allocator_->make<ProcedureCallNode>(position, callee, args);
}

ExprNode* Analyzer::analyzeIf(Value value, const Position& position) {
    if (!isPair(value))
        throw AnalyzeError(position, "invalid syntax of 'if'");
    PairObject* p1 = static_cast<PairObject*>(value.asPointer());
    if (!isPair(p1->getCdr()))
        throw AnalyzeError(position, "invalid syntax of 'if'");
    PairObject* p2 = static_cast<PairObject*>(p1->getCdr().asPointer());
    if (!isPair(p2->getCdr()))
        throw AnalyzeError(position, "invalid syntax of 'if'");
    PairObject* p3 = static_cast<PairObject*>(p2->getCdr().asPointer());
    if (p3->getCdr() != Value::Nil)
        throw AnalyzeError(position, "invalid syntax of 'if'");

    ExprNode* cond_node = analyzeExpr(p1->getCar(), source_map_->at(p1));
    ExprNode* then_node = analyzeExpr(p2->getCar(), source_map_->at(p2));
    ExprNode* else_node = analyzeExpr(p3->getCar(), source_map_->at(p3));
    return allocator_->make<IfNode>(position, cond_node, then_node, else_node);
}

ExprNode* Analyzer::analyzeAssignment(Value value, const Position& position) {
    if (!isPair(value))
        throw AnalyzeError(position, "invalid syntax of 'set!'");
    PairObject* p1 = static_cast<PairObject*>(value.asPointer());
    if (!isPair(p1->getCdr()))
        throw AnalyzeError(position, "invalid syntax of 'set!'");
    PairObject* p2 = static_cast<PairObject*>(p1->getCdr().asPointer());
    if (p2->getCdr() != Value::Nil)
        throw AnalyzeError(position, "invalid syntax of 'set!'");

    if (!p1->getCar().isSymbol())
        throw AnalyzeError(source_map_->at(p1), "the first argument of set! must be a symbol");
    Symbol name = p1->getCar().asSymbol();
    ExprNode* expr = analyzeExpr(p2->getCar(), source_map_->at(p2));
    return allocator_->make<AssignmentNode>(position, name, expr);
}

ExprNode* Analyzer::analyzeQuote(Value value, const Position& position) {
    if (!isPair(value))
        throw AnalyzeError(position, "invalid syntax of 'quote'");
    PairObject* p = static_cast<PairObject*>(value.asPointer());
    if (p->getCdr() != Value::Nil)
        throw AnalyzeError(position, "invalid syntax of quote");
    return allocator_->make<LiteralNode>(position, p->getCar());
}

}
