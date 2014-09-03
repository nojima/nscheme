#pragma once

#include "base_object.hpp"
#include <vector>

namespace nscheme {

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

}   // namespace nscheme
