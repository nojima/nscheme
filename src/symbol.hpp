#pragma once

#include <string>


namespace nscheme {


class Symbol {
public:
    Symbol(const std::string* name)
        : name_(name)
    {
    }

    const std::string& toString() const noexcept { return *name_; }

    uint64_t getInternalId() const noexcept { return reinterpret_cast<uint64_t>(name_); }

    bool operator==(const Symbol& rhs) const noexcept { return name_ == rhs.name_; }

    bool operator!=(const Symbol& rhs) const noexcept { return name_ != rhs.name_; }

private:
    const std::string* name_;
};


} // namespace nscheme


namespace std {


template <> struct hash<nscheme::Symbol> : public std::unary_function<nscheme::Symbol, size_t> {
    size_t operator()(const nscheme::Symbol& symbol) const
    {
        return hash<std::string>()(symbol.toString());
    }
};


} // namespace std
