#include "object.hpp"

namespace nscheme {

std::string ObjectToString(const Object* obj) {
    return obj->ToString();
}

bool IsNil(const Object* obj) {
    return dynamic_cast<const NilObject*>(obj) != nullptr;
}

bool IsPair(const Object* obj) {
    return dynamic_cast<const PairObject*>(obj) != nullptr;
}

}   // namespace nscheme
