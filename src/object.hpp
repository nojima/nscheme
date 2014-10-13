#pragma once

#include <string>
#include <vector>
#include "value.hpp"
#include "frame.hpp"

namespace nscheme {

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

class LabelInst;

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

}
