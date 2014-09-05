#pragma once

#include <string>
#include "symbol.hpp"

namespace nscheme {

class Object;

class Value {
public:
    static const Value Nil;
    static const Value False;
    static const Value True;
    static const Value Undefined;

    static Value makeInteger(int64_t n) {
        return Value((n << kShift) | kFlagInteger);
    }

    static Value makeSymbol(Symbol symbol) {
        return Value(symbol.getInternalId() | kFlagSymbol);
    }

    static Value makeCharacter(uint32_t character) {
        return Value((character << kShift) | kFlagCharacter);
    }

    bool isPointer() const {
        return (value_ & ~kMask) == 0 | value_ > kUndefined;
    }

    bool isInteger() const {
        return (value_ & ~kMask) == kFlagInteger;
    }

    bool isSymbol() const {
        return (value_ & ~kMask) == kFlagSymbol;
    }

    bool isCharacter() const {
        return (value_ & ~kMask) == kFlagCharacter;
    }

    Object* asPointer() {
        return reinterpret_cast<Object*>(value_);
    }

    const Object* asPointer() const {
        return reinterpret_cast<const Object*>(value_);
    }

    int64_t asInteger() const {
        return value_ >> kShift;
    }

    Symbol asSymbol() const {
        return Symbol(reinterpret_cast<const std::string*>(value_ & ~kMask));
    }

    int32_t asCharacter() const {
        return value_ >> kShift;
    }

    bool asBoolean() const {
        return (value_ & ~kFalse) == 0;
    }

    std::string toString() const;

    bool operator==(const Value& rhs) const noexcept {
        return value_ == rhs.value_;
    }

    bool operator!=(const Value& rhs) const noexcept {
        return value_ != rhs.value_;
    }

private:
    explicit Value(uint64_t value): value_(value) {}

    static constexpr uint64_t kNil = 0;
    static constexpr uint64_t kFalse = 4;
    static constexpr uint64_t kTrue = 8;
    static constexpr uint64_t kUndefined = 16;
    static constexpr uint64_t kFlagInteger = 1;
    static constexpr uint64_t kFlagSymbol = 2;
    static constexpr uint64_t kFlagCharacter = 3;
    static constexpr uint64_t kShift = 2;
    static constexpr uint64_t kMask = (1 << kShift) - 1;

    uint64_t value_;
};

}
