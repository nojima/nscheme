#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include "symbol.hpp"

namespace nscheme {

class Object {
public:
    virtual ~Object() {}

    virtual std::string ToString() const = 0;
};

std::string ObjectToString(const Object* obj);
bool IsNil(const Object* obj);
bool IsPair(const Object* obj);

class BooleanObject: public Object {
public:
    explicit BooleanObject(bool value): value_(value) {}

    bool Value() const noexcept {
        return value_;
    }

    std::string ToString() const {
        return value_ ? "#t" : "#f";
    }

private:
    bool value_;
};

class ByteVectorObject: public Object {
public:
    explicit ByteVectorObject(size_t size, std::uint8_t fill):
        data_(size, fill) {}

    std::string ToString() const;

private:
    std::vector<std::uint8_t> data_;
};


class CharacterObject: public Object {
public:
    explicit CharacterObject(char value): value_(value) {}

    char Value() const noexcept {
        return value_;
    }

    std::string ToString() const;

private:
    char value_;
};

class NilObject: public Object {
public:
    NilObject() {}

    std::string ToString() const {
        return "'()";
    }
};

class IntegerObject: public Object {
public:
    explicit IntegerObject(std::int64_t value): value_(value) {}

    std::int64_t Value() const noexcept {
        return value_;
    }

    std::string ToString() const {
        return std::to_string(value_);
    }

private:
    std::int64_t value_;
};

class PairObject: public Object {
public:
    PairObject(Object* car, Object* cdr):
        car_(car), cdr_(cdr) {}

    Object* Car() {
        return car_;
    }

    const Object* Car() const {
        return car_;
    }

    void SetCar(Object* car) {
        car_ = car;
    }

    Object* Cdr() {
        return cdr_;
    }

    const Object* Cdr() const {
        return cdr_;
    }

    void SetCdr(Object* cdr) {
        cdr_ = cdr;
    }

    std::string ToString() const;

private:
    Object* car_;
    Object* cdr_;
};

class StringObject: public Object {
public:
    explicit StringObject(const std::string& value): value_(value) {}

    const std::string& Value() const noexcept {
        return value_;
    }

    std::string ToString() const;

private:
    std::string value_;
};

class SymbolObject: public Object {
    explicit SymbolObject(Symbol symbol): value_(symbol) {}

    Symbol Value() const noexcept {
        return value_;
    }

    std::string ToString() const {
        return value_.ToString();
    }

private:
    Symbol value_;
};

}   // namespace nscheme
