#include "object.hpp"
#include <cctype>
#include <cstdio>


namespace nscheme {


void PairObject::mark()
{
    if (marked_)
        return;
    marked_ = true;
    if (car_.isPointer())
        car_.asPointer()->mark();
    if (cdr_.isPointer())
        cdr_.asPointer()->mark();
}


std::string StringObject::toString() const
{
    std::string buffer("\"");
    for (char ch : str_) {
        switch (ch) {
        case '"':
        case '\\':
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
            }
            else {
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


std::string PairObject::toString() const
{
    std::string buffer("(");
    const PairObject* obj = this;
    for (size_t i = 0;; ++i) {
        if (i != 0)
            buffer.push_back(' ');
        buffer += obj->car_.toString();

        if (obj->cdr_ == Value::Nil)
            break;
        if (!cdr_.isPointer()
            || dynamic_cast<const PairObject*>(obj->cdr_.asPointer()) == nullptr) {
            buffer += " . ";
            buffer += obj->cdr_.toString();
            break;
        }
        obj = static_cast<const PairObject*>(obj->cdr_.asPointer());
    }
    buffer += ")";
    return buffer;
}


std::string VectorObject::toString() const
{
    std::string buffer("#(");
    for (size_t i = 0; i < values_.size(); ++i) {
        if (i != 0)
            buffer.push_back(' ');
        buffer += values_[i].toString();
    }
    buffer.push_back(')');
    return buffer;
}


void VectorObject::mark()
{
    if (marked_)
        return;
    marked_ = true;
    for (Value v : values_) {
        if (v.isPointer())
            v.asPointer()->mark();
    }
}


void Frame::mark()
{
    if (marked_)
        return;
    marked_ = true;
    if (parent_)
        parent_->mark();
    for (auto v : variables_) {
        if (v.isPointer())
            v.asPointer()->mark();
    }
}


void ClosureObject::mark()
{
    if (marked_)
        return;
    marked_ = true;
    frame_->mark();
}


void CFunctionObject::mark() { marked_ = true; }

void ContinuationObject::mark()
{
    if (marked_)
        return;
    marked_ = true;
    for (Value v : value_stack_) {
        if (v.isPointer())
            v.asPointer()->mark();
    }
    for (Frame* f : frame_stack_) {
        f->mark();
    }
}


} // namespace nscheme
