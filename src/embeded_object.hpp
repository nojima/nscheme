#pragma once

#include <cassert>
#include "base_object.hpp"
#include "symbol.hpp"

namespace nscheme {

enum class ValueTag: std::uint64_t {
    kNil = 0,
    kInteger = 1,
    kTrue = 2,
    kSymbol = 2,
    kCharacter = 3,
    kFalse = 4,
};

inline bool IsNil(ObjectRef obj) {
    return obj == static_cast<ObjectRef>(ValueTag::kNil);
}

inline ObjectRef GetNilObject() {
    return static_cast<ObjectRef>(ValueTag::kNil);
}

inline bool IsBoolean(ObjectRef obj) {
    return obj == static_cast<ObjectRef>(ValueTag::kTrue) ||
           obj == static_cast<ObjectRef>(ValueTag::kFalse);
}

inline bool GetBooleanValue(ObjectRef obj) {
    assert(IsBoolean(obj));
    return obj != static_cast<ObjectRef>(ValueTag::kFalse);
}

inline ObjectRef GetBooleanObject(bool b) {
    if (b)
        return static_cast<ObjectRef>(ValueTag::kTrue);
    else
        return static_cast<ObjectRef>(ValueTag::kFalse);
}

inline bool IsCharacter(ObjectRef obj) {
    return (obj & 3) == static_cast<ObjectRef>(ValueTag::kCharacter);
}

inline char GetCharacterValue(ObjectRef obj) {
    assert(IsCharacter(obj));
    return static_cast<char>(obj >> 2);
}

inline ObjectRef GetCharacterObject(char ch) {
    return (ch << 2) | static_cast<ObjectRef>(ValueTag::kCharacter);
}

inline bool IsInteger(ObjectRef obj) {
    return (obj & 3) == static_cast<ObjectRef>(ValueTag::kInteger);
}

inline std::int64_t GetIntegerValue(ObjectRef obj) {
    assert(IsInteger(obj));
    return static_cast<std::int64_t>(obj) >> 2;
}

inline ObjectRef GetIntegerObject(std::int64_t n) {
    return (n << 2) | static_cast<ObjectRef>(ValueTag::kInteger);
}

inline bool IsSymbol(ObjectRef obj) {
    return (obj & 3) == static_cast<ObjectRef>(ValueTag::kSymbol);
}

inline Symbol GetSymbolValue(ObjectRef obj) {
    assert(IsSymbol(obj));
    return Symbol(reinterpret_cast<const std::string*>(obj & ~static_cast<ObjectRef>(3)));
}

inline ObjectRef GetSymbolObject(Symbol symbol) {
    return static_cast<ObjectRef>(symbol.InternalId() | static_cast<intptr_t>(ValueTag::kSymbol));
}

}   // namespace nscheme
