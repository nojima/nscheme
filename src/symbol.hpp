#pragma once

#include <string>
#include <unordered_set>

namespace nscheme {

class Symbol {
public:
    bool operator==(const Symbol& rhs) const noexcept {
        return p_ == rhs.p_;
    }

    bool operator!=(const Symbol& rhs) const noexcept {
        return p_ != rhs.p_;
    }

    const std::string& toString() const {
        return *p_;
    }

private:
    explicit Symbol(const std::string* p):
        p_(p) {}

    const std::string* p_;

    friend class SymbolTable;
};

class SymbolTable {
public:
    Symbol get(const std::string& name) {
        auto pair = table_.insert(name);
        return Symbol(&*pair.first);
    }

private:
    std::unordered_set<std::string> table_;
};

}   // namespace nscheme
