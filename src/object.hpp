#pragma once

#include <string>
#include <vector>
#include "value.hpp"

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

}
