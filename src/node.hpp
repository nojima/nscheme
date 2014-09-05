#pragma once

#include <string>
#include "position.hpp"
#include "value.hpp"
#include "object.hpp"

namespace nscheme {

class Node: public Object {
public:
    Node(const Position& position): position_(position) {}

    const Position& getPosition() const noexcept {
        return position_;
    }

private:
    Position position_;
};

class ExprNode: public Node {
public:
    ExprNode(const Position& position): Node(position) {}
};

class VariableNode: public ExprNode {
public:
    VariableNode(const Position& position, Symbol name)
        : ExprNode(position), name_(name) {}

    std::string toString() const override {
        return name_.toString();
    }

private:
    Symbol name_;
};

class LiteralNode: public ExprNode {
public:
    explicit LiteralNode(const Position& position, Value value)
        : ExprNode(position), value_(value) {}

    std::string toString() const override {
        if (isSelfEvaluating(value_))
            return value_.toString();
        else
            return "'" + value_.toString();
    }

private:
    static bool isSelfEvaluating(Value value) {
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
        }
        return false;
    }

    Value value_;
};

class ProcedureCallNode: public ExprNode {
public:
    ProcedureCallNode(const Position& position, ExprNode* callee,
                      const std::vector<ExprNode*>& operand)
        : ExprNode(position), callee_(callee), operand_(operand) {}

    std::string toString() const override {
        std::string buffer("(");
        buffer += callee_->toString();
        for (Node* p : operand_) {
            buffer.push_back(' ');
            buffer += p->toString();
        }
        buffer.push_back(')');
        return buffer;
    }

private:
    ExprNode* callee_;
    std::vector<ExprNode*> operand_;
};

class DefineNode: public Node {
public:
    DefineNode(const Position& position, Symbol name, ExprNode* expr)
        : Node(position), name_(name), expr_(expr) {}

    Symbol getName() const noexcept {
        return name_;
    }

    std::string toString() const override {
        std::string buffer("(define ");
        buffer += name_.toString();
        buffer.push_back(' ');
        buffer += expr_->toString();
        buffer.push_back(')');
        return buffer;
    }

private:
    Symbol name_;
    ExprNode* expr_;
};

class LambdaNode: public ExprNode {
public:
    LambdaNode(const Position& position,
               const std::vector<Symbol>& arg_names, bool variable_args,
               const std::vector<Symbol>& local_names,
               const std::vector<Node*>& nodes)
        : ExprNode(position)
        , arg_names_(arg_names), variable_args_(variable_args)
        , local_names_(local_names), nodes_(nodes) {}

    std::string toString() const override;

private:
    std::vector<Symbol> arg_names_;
    bool variable_args_;
    std::vector<Symbol> local_names_;
    std::vector<Node*> nodes_;
};

class IfNode: public ExprNode {
public:
    IfNode(const Position& position,
           ExprNode* cond_node, ExprNode* then_node, ExprNode* else_node)
        : ExprNode(position), cond_node_(cond_node)
        , then_node_(then_node), else_node_(else_node) {}

    std::string toString() const override {
        std::string buffer("(if ");
        buffer += cond_node_->toString();
        buffer.push_back(' ');
        buffer += then_node_->toString();
        buffer.push_back(' ');
        buffer += else_node_->toString();
        buffer.push_back(')');
        return buffer;
    }

private:
    ExprNode* cond_node_;
    ExprNode* then_node_;
    ExprNode* else_node_;
};

class AssignmentNode: public ExprNode {
public:
    AssignmentNode(const Position& position, Symbol name, ExprNode* expr)
        : ExprNode(position), name_(name), expr_(expr) {}

    std::string toString() const override {
        std::string buffer("(set! ");
        buffer += name_.toString();
        buffer.push_back(' ');
        buffer += expr_->toString();
        buffer.push_back(')');
        return buffer;
    }

private:
    Symbol name_;
    ExprNode* expr_;
};

}
