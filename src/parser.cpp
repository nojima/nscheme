#include "parser.hpp"


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


}   // namespace


namespace nscheme {


Node* Parser::parse(Value datum) {
    Position dummy(symbol_table_->intern(""), 1, 1);

    LocalNames names;
    names.parent = nullptr;
    for (size_t i = 0; i < global_names_->size(); ++i)
        names.name2index.insert(std::make_pair((*global_names_)[i], i));

    return parseExprOrDefine(datum, dummy, names);
}


Node* Parser::parseExprOrDefine(Value value, const Position& position, LocalNames& names) {
    if (isPair(value)) {
        PairObject* p = static_cast<PairObject*>(value.asPointer());
        Value head = p->getCar();
        if (head == Value::fromSymbol(kwd_define_))
            return parseDefine(p->getCdr(), source_map_->at(p), names);
    }
    return parseExpr(value, position, names);
}


ExprNode* Parser::parseExpr(Value value, const Position& position, LocalNames& names) {
    if (value.isSymbol()) {
        return allocator_->make<VariableNode>(position, value.asSymbol());
    }
    if (isSelfEvaluating(value)) {
        return allocator_->make<LiteralNode>(position, value);
    }

    if (!value.isPointer())
        throw ParseError(position, "invalid expression");
    PairObject* p = dynamic_cast<PairObject*>(value.asPointer());
    if (p == nullptr)
        throw ParseError(position, "invalid expression");

    Value head = p->getCar();
    if (head == Value::fromSymbol(kwd_lambda_)) {
        return parseLambda(p->getCdr(), source_map_->at(p), names);
    }
    if (head == Value::fromSymbol(kwd_if_)) {
        return parseIf(p->getCdr(), source_map_->at(p), names);
    }
    if (head == Value::fromSymbol(kwd_set_bang_)) {
        return parseAssignment(p->getCdr(), source_map_->at(p), names);
    }
    if (head == Value::fromSymbol(kwd_quote_)) {
        return parseQuote(p->getCdr(), source_map_->at(p), names);
    }
    return parseProcedureCall(p, source_map_->at(p), names);
}


ExprNode* Parser::parseLambda(Value value, const Position& position, LocalNames& names) {
    if (!isPair(value))
        throw ParseError(position, "invalid syntax of 'lambda'");
    PairObject* p1 = static_cast<PairObject*>(value.asPointer());
    if (!isPair(p1->getCdr()))
        throw ParseError(position, "invalid syntax of 'lambda'");

    bool variable = false;
    std::vector<Symbol> args;

    Value v1 = p1->getCar();
    if (v1.isSymbol()) {
        variable = true;
        args.push_back(v1.asSymbol());
    } else if (isPair(v1)) {
        Value v = v1;
        while (isPair(v)) {
            PairObject* p = static_cast<PairObject*>(v.asPointer());
            Value arg = p->getCar();
            if (!arg.isSymbol())
                throw ParseError(source_map_->at(p), "argument name must be a symbol");
            args.push_back(arg.asSymbol());
            v = p->getCdr();
        }
        if (v != Value::Nil) {
            if (!v.isSymbol())
                throw ParseError(position, "argument name must be a symbol");
            variable = true;
            args.push_back(v.asSymbol());
        }
    }

    LocalNames local_names;
    local_names.parent = &names;
    for (size_t i = 0; i < args.size(); ++i)
        local_names.name2index.insert(std::make_pair(args[i], i));

    std::vector<Node*> nodes;
    Value v = p1->getCdr();
    while (v != Value::Nil) {
        if (!isPair(v))
            throw ParseError(position, "invalid lambda body");
        PairObject* p = static_cast<PairObject*>(v.asPointer());
        Node* node = parseExprOrDefine(p->getCar(), source_map_->at(p), local_names);
        nodes.push_back(node);
        v = p->getCdr();
    }
    return allocator_->make<LambdaNode>(position, std::move(args), variable,
                                        std::move(local_names), std::move(nodes));
}


ExprNode* Parser::parseProcedureCall(PairObject* list, const Position& position, LocalNames& names) {
    ExprNode* callee = parseExpr(list->getCar(), position, names);
    std::vector<ExprNode*> args;
    Value v = list->getCdr();
    while (v != Value::Nil) {
        if (!isPair(v))
            throw ParseError(position, "invalid procedure call");
        PairObject* p = static_cast<PairObject*>(v.asPointer());
        args.push_back(parseExpr(p->getCar(), source_map_->at(p), names));
        v = p->getCdr();
    }
    return allocator_->make<ProcedureCallNode>(position, callee, args);
}


ExprNode* Parser::parseIf(Value value, const Position& position, LocalNames& names) {
    if (!isPair(value))
        throw ParseError(position, "invalid syntax of 'if'");
    PairObject* p1 = static_cast<PairObject*>(value.asPointer());
    if (!isPair(p1->getCdr()))
        throw ParseError(position, "invalid syntax of 'if'");
    PairObject* p2 = static_cast<PairObject*>(p1->getCdr().asPointer());
    if (!isPair(p2->getCdr()))
        throw ParseError(position, "invalid syntax of 'if'");
    PairObject* p3 = static_cast<PairObject*>(p2->getCdr().asPointer());
    if (p3->getCdr() != Value::Nil)
        throw ParseError(position, "invalid syntax of 'if'");

    ExprNode* cond_node = parseExpr(p1->getCar(), source_map_->at(p1), names);
    ExprNode* then_node = parseExpr(p2->getCar(), source_map_->at(p2), names);
    ExprNode* else_node = parseExpr(p3->getCar(), source_map_->at(p3), names);
    return allocator_->make<IfNode>(position, cond_node, then_node, else_node);
}


ExprNode* Parser::parseAssignment(Value value, const Position& position, LocalNames& names) {
    if (!isPair(value))
        throw ParseError(position, "invalid syntax of 'set!'");
    PairObject* p1 = static_cast<PairObject*>(value.asPointer());
    if (!isPair(p1->getCdr()))
        throw ParseError(position, "invalid syntax of 'set!'");
    PairObject* p2 = static_cast<PairObject*>(p1->getCdr().asPointer());
    if (p2->getCdr() != Value::Nil)
        throw ParseError(position, "invalid syntax of 'set!'");

    if (!p1->getCar().isSymbol())
        throw ParseError(source_map_->at(p1), "the first argument of set! must be a symbol");
    Symbol name = p1->getCar().asSymbol();
    ExprNode* expr = parseExpr(p2->getCar(), source_map_->at(p2), names);
    return allocator_->make<AssignmentNode>(position, name, expr);
}


ExprNode* Parser::parseQuote(Value value, const Position& position, LocalNames& names) {
    if (!isPair(value))
        throw ParseError(position, "invalid syntax of 'quote'");
    PairObject* p = static_cast<PairObject*>(value.asPointer());
    if (p->getCdr() != Value::Nil)
        throw ParseError(position, "invalid syntax of 'quote'");
    return allocator_->make<LiteralNode>(position, p->getCar());
}


Node* Parser::parseDefine(Value value, const Position& position, LocalNames& names) {
    if (!isPair(value))
        throw ParseError(position, "invalid syntax of 'define'");
    PairObject* p1 = static_cast<PairObject*>(value.asPointer());
    if (!isPair(p1->getCdr()))
        throw ParseError(position, "invalid syntax of 'define'");
    PairObject* p2 = static_cast<PairObject*>(p1->getCdr().asPointer());
    if (p2->getCdr() != Value::Nil)
        throw ParseError(position, "invalid syntax of 'define'");

    Value v1 = p1->getCar();
    if (v1.isSymbol()) {
        Symbol name = v1.asSymbol();
        size_t index = names.name2index.size();
        names.name2index.insert(std::make_pair(name, index));
        ExprNode* expr = parseExpr(p2->getCar(), source_map_->at(p2), names);
        return allocator_->make<DefineNode>(position, name, expr);
    } else if (isPair(v1)) {
        throw ParseError(position, "not implemented");
    } else {
        throw ParseError(position, "invalid argument of 'define'");
    }
}


}   // namespace nscheme
