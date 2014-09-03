#pragma once

#include <cassert>
#include <string>
#include <unordered_set>

namespace nscheme {

class Symbol {
public:
    explicit Symbol(const std::string* p): p_(p) {}

    bool operator==(const Symbol& rhs) const noexcept {
        return p_ == rhs.p_;
    }

    bool operator!=(const Symbol& rhs) const noexcept {
        return p_ != rhs.p_;
    }

    const std::string& ToString() const {
        return *p_;
    }

    intptr_t InternalId() const {
        return reinterpret_cast<intptr_t>(p_);
    }

private:
    const std::string* p_;

    friend class SymbolTable;
};

class SymbolTable {
public:
    Symbol Get(const std::string& name) {
        auto pair = table_.insert(name);
        return Symbol(&*pair.first);
    }

private:
    std::unordered_set<std::string> table_;
};

}   // namespace nscheme
