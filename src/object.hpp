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

typedef std::uint64_t ObjectRef;

std::string ObjectToString(ObjectRef obj);

inline bool IsRefObject(ObjectRef obj) {
    return (reinterpret_cast<std::uint64_t>(obj) & 3) == 0 &&
           reinterpret_cast<std::uint64_t>(obj) > 8;
}

template <typename T>
inline ObjectRef ref_cast(T* obj) {
    return reinterpret_cast<ObjectRef>(obj);
}

template <typename T>
inline T dynamic_ref_cast(ObjectRef obj) {
    return IsRefObject(obj) ? dynamic_cast<T>(reinterpret_cast<Object*>(obj)) : nullptr;
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

inline bool IsByteVector(ObjectRef obj) {
    return dynamic_ref_cast<const ByteVectorObject*>(obj) != nullptr;
}

class PairObject: public Object {
public:
    PairObject(ObjectRef car, ObjectRef cdr):
        car_(car), cdr_(cdr) {}

    ObjectRef Car() {
        return car_;
    }

    /*const*/ ObjectRef Car() const {
        return car_;
    }

    void SetCar(ObjectRef car) {
        car_ = car;
    }

    ObjectRef Cdr() {
        return cdr_;
    }

    /*const*/ ObjectRef Cdr() const {
        return cdr_;
    }

    void SetCdr(ObjectRef cdr) {
        cdr_ = cdr;
    }

    std::string ToString() const;

private:
    ObjectRef car_;
    ObjectRef cdr_;
};

inline bool IsPair(ObjectRef obj) {
    return dynamic_ref_cast<const PairObject*>(obj) != nullptr;
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

inline bool IsString(ObjectRef obj) {
    return dynamic_ref_cast<const StringObject*>(obj) != nullptr;
}

class VectorObject: public Object {
public:
    void Add(ObjectRef obj) {
        data_.push_back(obj);
    }

    std::string ToString() const;

private:
    std::vector<ObjectRef> data_;
};

inline bool IsVector(ObjectRef obj) {
    return dynamic_ref_cast<const VectorObject*>(obj) != nullptr;
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
inline bool IsNil(ObjectRef obj) {
    return obj == static_cast<ObjectRef>(ValueTag::kNil);
}
inline ObjectRef GetNilObject() {
    return static_cast<ObjectRef>(ValueTag::kNil);
}

class BooleanObject: public Object {};
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

class CharacterObject: public Object {};
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

class IntegerObject: public Object {};
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

class SymbolObject: public Object {};
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
