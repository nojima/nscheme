#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "value.hpp"


namespace nscheme {

class Inst;
class LabelInst;
struct Context;


class Object {
public:
    virtual ~Object() {}
    virtual std::string toString() const = 0;
};


class StringObject: public Object {
public:
    StringObject(const std::string& str)
        : str_(str) {}

    std::string toString() const override;

private:
    std::string str_;
};


class RealObject: public Object {
public:
    RealObject(double real): real_(real) {}

    std::string toString() const override {
        return std::to_string(real_);
    }

private:
    double real_;
};


class PairObject: public Object {
public:
    PairObject(Value car, Value cdr)
        : car_(car), cdr_(cdr) {}

    Value getCar() {
        return car_;
    }

    const Value getCar() const {
        return car_;
    }

    void setCar(Value car) {
        car_ = car;
    }

    Value getCdr() {
        return cdr_;
    }

    const Value getCdr() const {
        return cdr_;
    }

    void setCdr(Value cdr) {
        cdr_ = cdr;
    }

    std::string toString() const override;

private:
    Value car_;
    Value cdr_;
};


class VectorObject: public Object {
public:
    VectorObject() {}

    VectorObject(size_t length, Value fill)
        : values_(length, fill) {}

    size_t getLength() const noexcept {
        return values_.size();
    }

    Value get(size_t index) {
        return values_[index];
    }

    const Value get(size_t index) const {
        return values_[index];
    }

    void add(Value value) {
        values_.push_back(value);
    }

    void set(size_t index, Value value) {
        values_[index] = value;
    }

    std::string toString() const override;

private:
    std::vector<Value> values_;
};


class Frame: public Object {
public:
    Frame(Frame* parent, const std::unordered_map<Symbol, Value>& variables)
        : parent_(parent), variables_(variables) {}

    const Frame* getParent() const {
        return parent_;
    }

    Frame* getParent() {
        return parent_;
    }

    const std::unordered_map<Symbol, Value>& getVariables() const {
        return variables_;
    }

    std::unordered_map<Symbol, Value>& getVariables() {
        return variables_;
    }

    std::string toString() const {
        return "<frame>";
    }

private:
    Frame* parent_;
    std::unordered_map<Symbol, Value> variables_;
};


class ClosureObject: public Object {
public:
    ClosureObject(LabelInst* label, Frame* frame,
                  const std::vector<Symbol>& arg_names)
        : label_(label), frame_(frame), arg_names_(arg_names) {}

    LabelInst* getLabel() const noexcept {
        return label_;
    }

    Frame* getFrame() const noexcept {
        return frame_;
    }

    const std::vector<Symbol>& getArgNames() const noexcept {
        return arg_names_;
    }

    std::string toString() const override {
        return "<closure " + std::to_string((uintptr_t)label_) + ">";
    }

private:
    LabelInst* label_;
    Frame* frame_;
    std::vector<Symbol> arg_names_;
};


class CFunctionObject: public Object {
public:
    CFunctionObject(const std::function<void (Context*, size_t)>& func, const std::string& name)
        : func_(func), name_(name) {}

    void call(Context* ctx, size_t n_args) {
        func_(ctx, n_args);
    }

    std::string toString() const override {
        return "<c_function " + name_ + ">";
    }

private:
    std::function<void (Context*, size_t)> func_;
    std::string name_;
};


class ContinuationObject: public Object {
public:
    ContinuationObject(Inst** ip,
                       const std::vector<Value>& value_stack,
                       const std::vector<Inst**>& control_stack,
                       const std::vector<Frame*>& frame_stack)
        : ip_(ip), value_stack_(value_stack), control_stack_(control_stack)
        , frame_stack_(frame_stack) {}

    Inst** getInstrunctionPointer() {
        return ip_;
    }

    std::vector<Value>& getValueStack() {
        return value_stack_;
    }

    std::vector<Inst**>& getControlStack() {
        return control_stack_;
    }

    std::vector<Frame*>& getFrameStack() {
        return frame_stack_;
    }

    std::string toString() const override {
        return "<continuation>";
    }

private:
    Inst** ip_;
    std::vector<Value> value_stack_;
    std::vector<Inst**> control_stack_;
    std::vector<Frame*> frame_stack_;
};


}   // namespace nscheme
