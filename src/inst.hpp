#pragma once

#include <string>

#include "symbol.hpp"
#include "value.hpp"

namespace nscheme {

class Inst {
public:
    virtual ~Inst() {}

    virtual std::string toString() const = 0;
};

class LabelInst: public Inst {
public:
    LabelInst** getLocation() const noexcept {
        return location_;
    }

    void setLocation(LabelInst** location) {
        location_ = location;
    }

    std::string toString() const override {
        return "[" + std::to_string((uintptr_t)this) + "]";
    }

private:
    LabelInst** location_ = nullptr;
};

class LoadVariableInst: public Inst {
public:
    LoadVariableInst(Symbol name): name_(name) {}

    std::string toString() const override {
        return "  load_variable " + name_.toString();
    }

private:
    Symbol name_;
};

class LoadLiteralInst: public Inst {
public:
    LoadLiteralInst(Value value): value_(value) {}

    std::string toString() const override {
        return "  load_literal " + value_.toString();
    }

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

private:
    size_t n_args_;
};

class AssignInst: public Inst {
public:
    AssignInst(Symbol name): name_(name) {}

    std::string toString() const override {
        return "  assign " + name_.toString();
    }

private:
    Symbol name_;
};

class ReturnInst: public Inst {
public:
    std::string toString() const override {
        return "  return";
    }
};

class DiscardInst: public Inst {
public:
    std::string toString() const override {
        return "  discard";
    }
};

class BranchInst: public Inst {
public:
    BranchInst(LabelInst* then_label, LabelInst* else_label)
        : then_label_(then_label), else_label_(else_label) {}

    std::string toString() const override {
        return "  branch " + then_label_->toString() + " " + else_label_->toString();
    }

private:
    LabelInst* then_label_;
    LabelInst* else_label_;
};

}   // namespace nscheme
