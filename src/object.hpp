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
inline bool IsRefObject(const Object* obj) {
    return (reinterpret_cast<std::uint64_t>(obj) & 3) == 0 &&
           reinterpret_cast<std::uint64_t>(obj) > 8;
}

class ByteVectorObject: public Object {
public:
    explicit ByteVectorObject(size_t size, std::uint8_t fill):
        data_(size, fill) {}

    std::string ToString() const;

    bool operator==(const ByteVectorObject& rhs) const {
        return data_ == rhs.data_;
    }

private:
    std::vector<std::uint8_t> data_;
};

inline bool IsByteVector(const Object* obj) {
    return IsRefObject(obj) && dynamic_cast<const ByteVectorObject*>(obj) != nullptr;
}

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

inline bool IsPair(const Object* obj) {
    return IsRefObject(obj) && dynamic_cast<const PairObject*>(obj) != nullptr;
}

class StringObject: public Object {
public:
    explicit StringObject(const std::string& value): value_(value) {}

    const std::string& Value() const noexcept {
        return value_;
    }

    std::string ToString() const;

    bool operator==(const StringObject& rhs) const {
        return value_ == rhs.value_;
    }

private:
    std::string value_;
};

inline bool IsString(const Object* obj) {
    return IsRefObject(obj) && dynamic_cast<const StringObject*>(obj) != nullptr;
}

class VectorObject: public Object {
public:
    void Add(Object* obj) {
        data_.push_back(obj);
    }

    std::string ToString() const;

private:
    std::vector<Object*> data_;
};

inline bool IsVector(const Object* obj) {
    return IsRefObject(obj) && dynamic_cast<const VectorObject*>(obj) != nullptr;
}

enum class ValueTag: std::uint64_t {
    kNil = 0,
    kInteger = 1,
    kTrue = 2,
    kSymbol = 2,
    kCharacter = 3,
    kFalse = 4,
};

class NilObject: public Object {};
inline bool IsNil(const Object* obj) {
    return obj == reinterpret_cast<Object*>(ValueTag::kNil);
}
inline NilObject* GetNilObject() {
    return reinterpret_cast<NilObject*>(ValueTag::kNil);
}

class BooleanObject: public Object {};
inline bool IsBoolean(const Object* obj) {
    return obj == reinterpret_cast<Object*>(ValueTag::kTrue) ||
           obj == reinterpret_cast<Object*>(ValueTag::kFalse);
}
inline bool GetBooleanValue(const Object* obj) {
    assert(IsBoolean(obj));
    return obj != reinterpret_cast<Object*>(ValueTag::kFalse);
}
inline BooleanObject* GetBooleanObject(bool b) {
    if (b)
        return reinterpret_cast<BooleanObject*>(ValueTag::kTrue);
    else
        return reinterpret_cast<BooleanObject*>(ValueTag::kFalse);
}

class CharacterObject: public Object {};
inline bool IsCharacter(const Object* obj) {
    return (reinterpret_cast<std::uint64_t>(obj) & 3) == static_cast<std::uint64_t>(ValueTag::kCharacter);
}
inline char GetCharacterValue(const Object* obj) {
    assert(IsCharacter(obj));
    return static_cast<char>(reinterpret_cast<std::uint64_t>(obj) >> 2);
}
inline CharacterObject* GetCharacterObject(char&& ch) {
    return reinterpret_cast<CharacterObject*>((ch << 2) | static_cast<intptr_t>(ValueTag::kCharacter));
}

class IntegerObject: public Object {};
inline bool IsInteger(const Object* obj) {
    return (reinterpret_cast<std::uint64_t>(obj) & 3) == static_cast<std::uint64_t>(ValueTag::kInteger);
}
inline std::int64_t GetIntegerValue(const Object* obj) {
    assert(IsInteger(obj));
    return reinterpret_cast<std::int64_t>(obj) >> 2;
}
inline IntegerObject* GetIntegerObject(std::int64_t n) {
    return reinterpret_cast<IntegerObject*>((n << 2) | static_cast<std::int64_t>(ValueTag::kInteger));
}

class SymbolObject: public Object {};
inline bool IsSymbol(const Object* obj) {
    return (reinterpret_cast<std::uint64_t>(obj) & 3) == static_cast<std::uint64_t>(ValueTag::kSymbol);
}
inline Symbol GetSymbolValue(const Object* obj) {
    assert(IsSymbol(obj));
    return Symbol(
            reinterpret_cast<const std::string*>(
                reinterpret_cast<std::uint64_t>(obj) & ~static_cast<std::uint64_t>(3)));
}
inline SymbolObject* GetSymbolObject(Symbol symbol) {
    return reinterpret_cast<SymbolObject*>(symbol.InternalId() | static_cast<intptr_t>(ValueTag::kSymbol));
}

}   // namespace nscheme
