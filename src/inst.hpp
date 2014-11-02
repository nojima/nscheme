#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include "symbol.hpp"
#include "value.hpp"


namespace nscheme {

struct Context;


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

    Value getValue() const {
        return value_;
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
        if (tail_)
            return "  tail_apply " + std::to_string(n_args_);
        else
            return "  apply " + std::to_string(n_args_);
    }

    void setTail(bool tail) {
        tail_ = tail;
    }

    void exec(Context* context) override;

private:
    size_t n_args_;
    bool tail_ = false;
};


class DefineInst: public Inst {
public:
    DefineInst(Symbol name): name_(name) {}

    std::string toString() const override {
        return "  define " + name_.toString();
    }

    void exec(Context* context) override;

private:
    Symbol name_;
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


class JumpInst: public Inst {
public:
    JumpInst(LabelInst* label): label_(label) {}

    LabelInst* getLabel() noexcept {
        return label_;
    }

    const LabelInst* getLabel() const noexcept {
        return label_;
    }

    std::string toString() const override {
        return "  jump " + label_->toString();
    }

    void exec(Context* context) override;

private:
    LabelInst* label_;
};


class JumpIfInst: public Inst {
public:
    JumpIfInst(LabelInst* label): label_(label) {}

    LabelInst* getLabel() noexcept {
        return label_;
    }

    const LabelInst* getLabel() const noexcept {
        return label_;
    }

    std::string toString() const override {
        return "  jump_if " + label_->toString();
    }

    void exec(Context* context) override;

private:
    LabelInst* label_;
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
