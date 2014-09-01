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
    explicit ByteVectorObject(size_t size, char fill):
        data_(size, fill) {}

    std::string ToString() const {
        std::string buffer("#u8(");
        for (size_t i = 0; i < data_.size(); ++i) {
            if (i != 0)
                buffer.push_back(' ');
            buffer += std::to_string(data_[i]);
        }
        buffer.push_back(')');
        return buffer;
    }

private:
    std::vector<char> data_;
};


class CharacterObject: public Object {
public:
    explicit CharacterObject(char value): value_(value) {}

    char Value() const noexcept {
        return value_;
    }

    std::string ToString() const {
        if (isalnum(static_cast<unsigned char>(value_))) {
            return "#\\" + std::string(1, value_);
        } else {
            char buffer[16];
            std::snprintf(buffer, sizeof(buffer), "#\\x%02x", value_);
            return std::string(buffer);
        }
    }

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

    Object* Cdr() {
        return cdr_;
    }

    const Object* Cdr() const {
        return cdr_;
    }

    std::string ToString() const {
        std::string buffer("(");
        const PairObject* obj = this;
        bool first = true;
        for (;;) {
            if (!first)
                buffer.push_back(' ');
            else
                first = false;
            buffer += ObjectToString(obj->car_);

            if (IsNil(obj->cdr_))
                break;
            if (!IsPair(obj->cdr_)) {
                buffer += " . ";
                buffer += ObjectToString(obj->cdr_);
                break;
            }
            obj = static_cast<PairObject*>(obj->cdr_);
        }
        buffer += ")";
        return buffer;
    }

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

    std::string ToString() const {
        std::string buffer(1, '"');
        for (size_t i = 0; i < value_.size(); ++i) {
            char ch = value_[i];
            if (ch == '"' || ch == '\\') {
                buffer.push_back('\\');
                buffer.push_back(ch);
            } else if (std::isprint(ch)) {
                buffer.push_back(ch);
            } else {
                char tmp[16];
                snprintf(tmp, sizeof(tmp), "\\x%02x", ch);
                buffer += tmp;
            }
        }
        buffer.push_back('"');
        return buffer;
    }

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
