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

std::string ByteVectorObject::ToString() const {
    std::string buffer("#u8(");
    for (size_t i = 0; i < data_.size(); ++i) {
        if (i != 0)
            buffer.push_back(' ');
        buffer += std::to_string(data_[i]);
    }
    buffer.push_back(')');
    return buffer;
}

std::string CharacterObject::ToString() const {
    if (isalnum(static_cast<unsigned char>(value_))) {
        return "#\\" + std::string(1, value_);
    } else {
        char buffer[16];
        std::snprintf(buffer, sizeof(buffer), "#\\x%02x", value_);
        return std::string(buffer);
    }
}

std::string PairObject::ToString() const {
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

std::string StringObject::ToString() const {
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

}   // namespace nscheme
