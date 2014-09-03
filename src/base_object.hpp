#pragma once

#include <string>

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

}   // namespace nscheme
