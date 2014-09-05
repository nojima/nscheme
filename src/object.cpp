#include "object.hpp"
#include <cstdio>
#include <cctype>

namespace nscheme {

std::string StringObject::toString() const {
    std::string buffer("\"");
    for (char ch : str_) {
        switch (ch) {
        case '"': case '\\':
            buffer.push_back('\\');
            buffer.push_back(ch);
            break;
        case '\t':
            buffer += "\\t";
            break;
        case '\r':
            buffer += "\\r";
            break;
        case '\n':
            buffer += "\\n";
            break;
        default:
            if (isprint(ch)) {
                buffer.push_back(ch);
            } else {
                char tmp[16];
                std::snprintf(tmp, sizeof(tmp), "\\x%02x", ch);
                buffer += tmp;
            }
            break;
        }
    }
    buffer.push_back('"');
    return buffer;
}

std::string PairObject::toString() const {
    std::string buffer("(");
    const PairObject* obj = this;
    for (size_t i = 0; ; ++i) {
        if (i != 0)
            buffer.push_back(' ');
        buffer += obj->car_.toString();

        if (obj->cdr_ == Value::Nil)
            break;
        if (!cdr_.isPointer() ||
            dynamic_cast<const PairObject*>(obj->cdr_.asPointer()) == nullptr) {
            buffer += " . ";
            buffer += obj->cdr_.toString();
            break;
        }
        obj = static_cast<const PairObject*>(obj->cdr_.asPointer());
    }
    buffer += ")";
    return buffer;
}

std::string VectorObject::toString() const {
    std::string buffer("#(");
    for (size_t i = 0; i < values_.size(); ++i) {
        if (i != 0)
            buffer.push_back(' ');
        buffer += values_[i].toString();
    }
    buffer.push_back(')');
    return buffer;
}

}
