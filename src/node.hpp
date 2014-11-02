#pragma once

#include <string>
#include <memory>
#include "object.hpp"
#include "position.hpp"
#include "value.hpp"


namespace nscheme {

struct Code;


class Node {
public:
    Node(const Position& position): position_(position) {}
    virtual ~Node() {}

    const Position& getPosition() const noexcept {
        return position_;
    }

    virtual std::string toString() const = 0;
    virtual void codegen(Code& code) = 0;

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
    std::string toString() const override;
    void codegen(Code& code) override;

private:
    Symbol name_;
};


class LiteralNode: public ExprNode {
public:
    explicit LiteralNode(const Position& position, Value value)
        : ExprNode(position), value_(value) {}
    std::string toString() const override;
    void codegen(Code& code) override;

private:
    Value value_;
};


class ProcedureCallNode: public ExprNode {
public:
    ProcedureCallNode(const Position& position,
                      std::unique_ptr<ExprNode> callee,
                      std::vector<std::unique_ptr<ExprNode>>&& operand)
        : ExprNode(position)
        , callee_(std::move(callee))
        , operand_(std::move(operand)) {}
    std::string toString() const override;
    void codegen(Code& code) override;

private:
    std::unique_ptr<ExprNode> callee_;
    std::vector<std::unique_ptr<ExprNode>> operand_;
};


class DefineNode: public Node {
public:
    DefineNode(const Position& position, Symbol name,
               std::unique_ptr<ExprNode> expr)
        : Node(position), name_(name)
        , expr_(std::move(expr)) {}

    Symbol getName() const noexcept {
        return name_;
    }

    std::string toString() const override;
    void codegen(Code& code) override;

private:
    Symbol name_;
    std::unique_ptr<ExprNode> expr_;
};


class LambdaNode: public ExprNode {
public:
    LambdaNode(const Position& position,
               const std::vector<Symbol>& arg_names, bool variable_args,
               const std::vector<Symbol>& local_names,
               std::vector<std::unique_ptr<Node>>&& nodes)
        : ExprNode(position)
        , arg_names_(arg_names), variable_args_(variable_args)
        , local_names_(local_names), nodes_(std::move(nodes)) {}
    std::string toString() const override;
    void codegen(Code& code) override;

private:
    std::vector<Symbol> arg_names_;
    bool variable_args_;
    std::vector<Symbol> local_names_;
    std::vector<std::unique_ptr<Node>> nodes_;
};


class IfNode: public ExprNode {
public:
    IfNode(const Position& position,
           std::unique_ptr<ExprNode> cond_node,
           std::unique_ptr<ExprNode> then_node,
           std::unique_ptr<ExprNode> else_node)
        : ExprNode(position)
        , cond_node_(std::move(cond_node))
        , then_node_(std::move(then_node))
        , else_node_(std::move(else_node)) {}
    std::string toString() const override;
    void codegen(Code& code) override;

private:
    std::unique_ptr<ExprNode> cond_node_;
    std::unique_ptr<ExprNode> then_node_;
    std::unique_ptr<ExprNode> else_node_;
};


class AssignmentNode: public ExprNode {
public:
    AssignmentNode(const Position& position, Symbol name,
                   std::unique_ptr<ExprNode> expr)
        : ExprNode(position), name_(name)
        , expr_(std::move(expr)) {}
    std::string toString() const override;
    void codegen(Code& code) override;

private:
    Symbol name_;
    std::unique_ptr<ExprNode> expr_;
};


}   // namespace nscheme
