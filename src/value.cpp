#include "value.hpp"
#include "object.hpp"

namespace nscheme {

const Value Value::Nil(Value::kNil);
const Value Value::False(Value::kFalse);
const Value Value::True(Value::kTrue);
const Value Value::Undefined(Value::kUndefined);

std::string Value::toString() const {
    if (value_ == kNil) {
        return "()";
    }
    if (value_ == kFalse) {
        return "#f";
    }
    if (value_ == kTrue) {
        return "#t";
    }
    if (value_ == kUndefined) {
        return "<undef>";
    }
    if (isInteger()) {
        return std::to_string(asInteger());
    }
    if (isSymbol()) {
        return asSymbol().toString();
    }
    if (isCharacter()) {
        return std::string(1, asCharacter());
    }
    return asPointer()->toString();
}

}
