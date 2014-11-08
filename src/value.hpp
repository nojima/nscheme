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

    static Value fromPointer(Object* obj) { return Value(reinterpret_cast<uint64_t>(obj)); }

    static Value fromInteger(int64_t n)
    {
        return Value(static_cast<uint64_t>(n << kShift) | kFlagInteger);
    }

    static Value fromSymbol(Symbol symbol) { return Value(symbol.getInternalId() | kFlagSymbol); }

    static Value fromCharacter(uint32_t character)
    {
        return Value((static_cast<uint64_t>(character) << kShift) | kFlagCharacter);
    }

    static Value fromBoolean(bool b) { return b ? Value::True : Value::False; }

    bool isPointer() const { return (value_ & kMask) == 0 & value_ > kUndefined; }

    bool isInteger() const { return (value_ & kMask) == kFlagInteger; }

    bool isSymbol() const { return (value_ & kMask) == kFlagSymbol; }

    bool isCharacter() const { return (value_ & kMask) == kFlagCharacter; }

    Object* asPointer() { return reinterpret_cast<Object*>(value_); }

    const Object* asPointer() const { return reinterpret_cast<const Object*>(value_); }

    int64_t asInteger() const { return static_cast<int64_t>(value_) >> kShift; }

    Symbol asSymbol() const
    {
        return Symbol(reinterpret_cast<const std::string*>(value_ & ~kMask));
    }

    uint32_t asCharacter() const { return static_cast<uint32_t>(value_ >> kShift); }

    bool asBoolean() const { return !((value_ == kNil) | (value_ == kFalse)); }

    std::string toString() const;

    bool operator==(const Value& rhs) const noexcept { return value_ == rhs.value_; }

    bool operator!=(const Value& rhs) const noexcept { return value_ != rhs.value_; }

private:
    explicit Value(uint64_t value)
        : value_(value)
    {
    }

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


} // namespace nscheme
