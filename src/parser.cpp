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


template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}


}   // namespace


namespace nscheme {


std::unique_ptr<Node> Parser::parse(Value datum) {
    Position dummy(symbol_table_->intern(""), 1, 1);
    return parseExprOrDefine(datum, dummy);
}


std::unique_ptr<Node> Parser::parseExprOrDefine(Value value, const Position& position) {
    if (isPair(value)) {
        PairObject* p = static_cast<PairObject*>(value.asPointer());
        Value head = p->getCar();
        if (head == Value::fromSymbol(kwd_define_))
            return parseDefine(p->getCdr(), source_map_->at(p));
    }
    return parseExpr(value, position);
}


std::unique_ptr<ExprNode> Parser::parseExpr(Value value, const Position& position) {
    if (value.isSymbol()) {
        return make_unique<VariableNode>(position, value.asSymbol());
    }
    if (isSelfEvaluating(value)) {
        return make_unique<LiteralNode>(position, value);
    }

    if (!value.isPointer())
        throw ParseError(position, "invalid expression");
    PairObject* p = dynamic_cast<PairObject*>(value.asPointer());
    if (p == nullptr)
        throw ParseError(position, "invalid expression");

    Value head = p->getCar();
    if (head == Value::fromSymbol(kwd_lambda_)) {
        return parseLambda(p->getCdr(), source_map_->at(p));
    }
    if (head == Value::fromSymbol(kwd_if_)) {
        return parseIf(p->getCdr(), source_map_->at(p));
    }
    if (head == Value::fromSymbol(kwd_set_bang_)) {
        return parseAssignment(p->getCdr(), source_map_->at(p));
    }
    if (head == Value::fromSymbol(kwd_quote_)) {
        return parseQuote(p->getCdr(), source_map_->at(p));
    }
    return parseProcedureCall(p, source_map_->at(p));
}


std::unique_ptr<ExprNode> Parser::parseLambda(Value value, const Position& position) {
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

    std::vector<std::unique_ptr<Node>> nodes;
    std::vector<Symbol> local_names;
    Value v = p1->getCdr();
    while (v != Value::Nil) {
        if (!isPair(v))
            throw ParseError(position, "invalid lambda body");
        PairObject* p = static_cast<PairObject*>(v.asPointer());
        auto node = parseExprOrDefine(p->getCar(), source_map_->at(p));
        if (DefineNode* def = dynamic_cast<DefineNode*>(node.get())) {
            local_names.push_back(def->getName());
        }
        nodes.push_back(std::move(node));
        v = p->getCdr();
    }
    return make_unique<LambdaNode>(position, std::move(args), variable,
                                   std::move(local_names), std::move(nodes));
}


std::unique_ptr<ExprNode> Parser::parseProcedureCall(PairObject* list, const Position& position) {
    auto callee = parseExpr(list->getCar(), position);
    std::vector<std::unique_ptr<ExprNode>> args;
    Value v = list->getCdr();
    while (v != Value::Nil) {
        if (!isPair(v))
            throw ParseError(position, "invalid procedure call");
        PairObject* p = static_cast<PairObject*>(v.asPointer());
        auto expr = parseExpr(p->getCar(), source_map_->at(p));
        args.push_back(std::move(expr));
        v = p->getCdr();
    }
    return make_unique<ProcedureCallNode>(position, std::move(callee), std::move(args));
}


std::unique_ptr<ExprNode> Parser::parseIf(Value value, const Position& position) {
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

    auto cond_node = parseExpr(p1->getCar(), source_map_->at(p1));
    auto then_node = parseExpr(p2->getCar(), source_map_->at(p2));
    auto else_node = parseExpr(p3->getCar(), source_map_->at(p3));
    return make_unique<IfNode>(position,
                               std::move(cond_node),
                               std::move(then_node),
                               std::move(else_node));
}


std::unique_ptr<ExprNode> Parser::parseAssignment(Value value, const Position& position) {
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
    auto expr = parseExpr(p2->getCar(), source_map_->at(p2));
    return make_unique<AssignmentNode>(position, name, std::move(expr));
}


std::unique_ptr<ExprNode> Parser::parseQuote(Value value, const Position& position) {
    if (!isPair(value))
        throw ParseError(position, "invalid syntax of 'quote'");
    PairObject* p = static_cast<PairObject*>(value.asPointer());
    if (p->getCdr() != Value::Nil)
        throw ParseError(position, "invalid syntax of 'quote'");
    return make_unique<LiteralNode>(position, p->getCar());
}


std::unique_ptr<Node> Parser::parseDefine(Value value, const Position& position) {
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
        auto expr = parseExpr(p2->getCar(), source_map_->at(p2));
        return make_unique<DefineNode>(position, name, std::move(expr));
    } else if (isPair(v1)) {
        throw ParseError(position, "not implemented");
    } else {
        throw ParseError(position, "invalid argument of 'define'");
    }
}


}   // namespace nscheme
