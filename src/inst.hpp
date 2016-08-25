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


class LabelInst : public Inst {
public:
    Inst** getLocation() const noexcept { return location_; }

    void setLocation(Inst** location) { location_ = location; }

    std::string toString() const override { return "[" + std::to_string((uintptr_t) this) + "]"; }

    void exec(Context*) override;

private:
    Inst** location_ = nullptr;
};


class LoadNamedVariableInst : public Inst {
public:
    LoadNamedVariableInst(Symbol name)
        : name_(name)
    {
    }

    std::string toString() const override { return "  load_variable " + name_.toString(); }

    void exec(Context* context) override;

private:
    Symbol name_;
};


class LoadIndexedVariableInst : public Inst {
public:
    LoadIndexedVariableInst(size_t frame_index, size_t variable_index)
        : frame_index_(frame_index)
        , variable_index_(variable_index)
    {
    }

    std::string toString() const override
    {
        return "  load_indexed_variable " + std::to_string(frame_index_) + " "
               + std::to_string(variable_index_);
    }

    void exec(Context* context) override;

private:
    size_t frame_index_;
    size_t variable_index_;
};


class LoadLiteralInst : public Inst {
public:
    LoadLiteralInst(Value value)
        : value_(value)
    {
    }

    std::string toString() const override { return "  load_literal " + value_.toString(); }

    Value getValue() const { return value_; }

    void exec(Context* context) override;

private:
    Value value_;
};


class LoadClosureInst : public Inst {
public:
    LoadClosureInst(LabelInst* label, size_t arg_size, size_t frame_size)
        : label_(label)
        , arg_size_(arg_size)
        , frame_size_(frame_size)
    {
    }

    std::string toString() const override
    {
        std::string ret = "  load_closure " + label_->toString();
        ret += std::to_string(arg_size_);
        ret += " ";
        ret += std::to_string(frame_size_);
        return ret;
    }

    void exec(Context* context) override;

private:
    LabelInst* label_;
    size_t arg_size_;
    size_t frame_size_;
};


class ApplyInst : public Inst {
public:
    ApplyInst(size_t n_args)
        : n_args_(n_args)
    {
    }

    std::string toString() const override
    {
        if (tail_)
            return "  tail_apply " + std::to_string(n_args_);
        else
            return "  apply " + std::to_string(n_args_);
    }

    void setTail(bool tail) { tail_ = tail; }

    void exec(Context* context) override;

private:
    size_t n_args_;
    bool tail_ = false;
};


class NamedAssignInst : public Inst {
public:
    NamedAssignInst(Symbol name)
        : name_(name)
    {
    }

    std::string toString() const override { return "  named_assign " + name_.toString(); }

    void exec(Context* context) override;

private:
    Symbol name_;
};


class IndexedAssignInst : public Inst {
public:
    IndexedAssignInst(size_t frame_index, size_t variable_index)
        : frame_index_(frame_index)
        , variable_index_(variable_index)
    {
    }

    std::string toString() const override
    {
        return "  indexed_assign " + std::to_string(frame_index_) + " "
               + std::to_string(variable_index_);
    }

    void exec(Context* context) override;

private:
    size_t frame_index_;
    size_t variable_index_;
};


class ReturnInst : public Inst {
public:
    std::string toString() const override { return "  return"; }

    void exec(Context* context) override;
};


class DiscardInst : public Inst {
public:
    std::string toString() const override { return "  discard"; }

    void exec(Context* context) override;
};


class JumpInst : public Inst {
public:
    JumpInst(LabelInst* label)
        : label_(label)
    {
    }

    LabelInst* getLabel() noexcept { return label_; }

    const LabelInst* getLabel() const noexcept { return label_; }

    std::string toString() const override { return "  jump " + label_->toString(); }

    void exec(Context* context) override;

private:
    LabelInst* label_;
};


class JumpIfInst : public Inst {
public:
    JumpIfInst(LabelInst* label)
        : label_(label)
    {
    }

    LabelInst* getLabel() noexcept { return label_; }

    const LabelInst* getLabel() const noexcept { return label_; }

    std::string toString() const override { return "  jump_if " + label_->toString(); }

    void exec(Context* context) override;

private:
    LabelInst* label_;
};


class QuitInst : public Inst {
public:
    std::string toString() const override { return "  quit"; }

    void exec(Context* context) override;
};


struct NameError : public std::runtime_error {
    NameError(const std::string& message)
        : std::runtime_error(message)
    {
    }
};


struct TypeError : public std::runtime_error {
    TypeError(const std::string& message)
        : std::runtime_error(message)
    {
    }
};


struct Quit {
};


} // namespace nscheme
