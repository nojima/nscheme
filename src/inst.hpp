#pragma once

#include <string>
#include <stdexcept>

#include "symbol.hpp"
#include "value.hpp"
#include "context.hpp"

namespace nscheme {

class Inst {
public:
    virtual ~Inst() {}
    virtual std::string toString() const = 0;
    virtual void exec(Context* context) = 0;
};

class LabelInst: public Inst {
public:
    Inst** getLocation() const noexcept {
        return location_;
    }

    void setLocation(Inst** location) {
        location_ = location;
    }

    std::string toString() const override {
        return "[" + std::to_string((uintptr_t)this) + "]";
    }

    void exec(Context*) override;

private:
    Inst** location_ = nullptr;
};

class LoadVariableInst: public Inst {
public:
    LoadVariableInst(Symbol name): name_(name) {}

    std::string toString() const override {
        return "  load_variable " + name_.toString();
    }

    void exec(Context* context) override;

private:
    Symbol name_;
};

class LoadLiteralInst: public Inst {
public:
    LoadLiteralInst(Value value): value_(value) {}

    std::string toString() const override {
        return "  load_literal " + value_.toString();
    }

    void exec(Context* context) override;

private:
    Value value_;
};

class LoadClosureInst: public Inst {
public:
    LoadClosureInst(LabelInst* label, const std::vector<Symbol>& args)
        : label_(label), args_(args) {}

    std::string toString() const override {
        std::string ret = "  load_closure " + label_->toString();
        for (Symbol arg: args_) {
            ret += ' ';
            ret += arg.toString();
        }
        return std::move(ret);
    }

    void exec(Context* context) override;

private:
    LabelInst* label_;
    std::vector<Symbol> args_;
};

class ApplyInst: public Inst {
public:
    ApplyInst(size_t n_args): n_args_(n_args) {}

    std::string toString() const override {
        return "  apply " + std::to_string(n_args_);
    }

    void exec(Context* context) override;

private:
    size_t n_args_;
};

class AssignInst: public Inst {
public:
    AssignInst(Symbol name): name_(name) {}

    std::string toString() const override {
        return "  assign " + name_.toString();
    }

    void exec(Context* context) override;

private:
    Symbol name_;
};

class ReturnInst: public Inst {
public:
    std::string toString() const override {
        return "  return";
    }

    void exec(Context* context) override;
};

class DiscardInst: public Inst {
public:
    std::string toString() const override {
        return "  discard";
    }

    void exec(Context* context) override;
};

class BranchInst: public Inst {
public:
    BranchInst(LabelInst* then_label, LabelInst* else_label)
        : then_label_(then_label), else_label_(else_label) {}

    std::string toString() const override {
        return "  branch " + then_label_->toString() + " " + else_label_->toString();
    }

    void exec(Context* context) override;

private:
    LabelInst* then_label_;
    LabelInst* else_label_;
};

class BranchReturnInst: public Inst {
public:
    std::string toString() const override {
        return "  branch_return";
    }

    void exec(Context* context) override;
};

class QuitInst: public Inst {
public:
    std::string toString() const override {
        return "  quit";
    }

    void exec(Context* context) override;
};

struct NameError: public std::runtime_error {
    NameError(const std::string& message): std::runtime_error(message) {}
};

struct TypeError: public std::runtime_error {
    TypeError(const std::string& message): std::runtime_error(message) {}
};

struct Quit {};

}   // namespace nscheme
