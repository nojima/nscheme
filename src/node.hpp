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


class NamedVariableNode: public ExprNode {
public:
    NamedVariableNode(const Position& position, Symbol name)
        : ExprNode(position), name_(name) {}
    std::string toString() const override;
    void codegen(Code& code) override;

private:
    Symbol name_;
};


class IndexedVariableNode: public ExprNode {
public:
    IndexedVariableNode(const Position& position,
                        size_t frame_index, size_t variable_index)
        : ExprNode(position)
        , frame_index_(frame_index), variable_index_(variable_index) {}
    std::string toString() const override;
    void codegen(Code& code) override;

private:
    size_t frame_index_;
    size_t variable_index_;
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
    DefineNode(const Position& position, Symbol name, size_t index,
               Value unparsed_expr, const Position& expr_position)
        : Node(position), name_(name), index_(index)
        , unparsed_expr_(unparsed_expr)
        , expr_position_(expr_position)
        {}

    Symbol getName() const noexcept {
        return name_;
    }

    Value getUnparsedExpr() const noexcept {
        return unparsed_expr_;
    }

    const Position& getUnparsedExprPosition() const noexcept {
        return expr_position_;
    }

    void setExpr(std::unique_ptr<ExprNode>&& expr) {
        expr_ = std::move(expr);
    }

    std::string toString() const override;
    void codegen(Code& code) override;

private:
    Symbol name_;
    size_t index_;
    std::unique_ptr<ExprNode> expr_;
    Value unparsed_expr_;
    Position expr_position_;
};


class LambdaNode: public ExprNode {
public:
    LambdaNode(const Position& position,
               const std::vector<Symbol>& arg_names,
               bool variable_args,
               size_t frame_size,
               std::vector<std::unique_ptr<Node>>&& nodes)
        : ExprNode(position)
        , arg_names_(arg_names)
        , variable_args_(variable_args)
        , frame_size_(frame_size)
        , nodes_(std::move(nodes)) {}
    std::string toString() const override;
    void codegen(Code& code) override;

private:
    std::vector<Symbol> arg_names_;
    bool variable_args_;
    size_t frame_size_;
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


class NamedAssignmentNode: public ExprNode {
public:
    NamedAssignmentNode(const Position& position, Symbol name,
                       std::unique_ptr<ExprNode> expr)
        : ExprNode(position), name_(name)
        , expr_(std::move(expr)) {}
    std::string toString() const override;
    void codegen(Code& code) override;

private:
    Symbol name_;
    std::unique_ptr<ExprNode> expr_;
};


class IndexedAssignmentNode: public ExprNode {
public:
    IndexedAssignmentNode(const Position& position,
                          size_t frame_index,
                          size_t variable_index,
                          std::unique_ptr<ExprNode> expr)
        : ExprNode(position)
        , frame_index_(frame_index)
        , variable_index_(variable_index)
        , expr_(std::move(expr)) {}
    std::string toString() const override;
    void codegen(Code& code) override;

private:
    size_t frame_index_;
    size_t variable_index_;
    std::unique_ptr<ExprNode> expr_;
};


}   // namespace nscheme
