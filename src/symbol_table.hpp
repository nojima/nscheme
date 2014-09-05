#pragma once

#include <string>
#include <unordered_set>
#include "symbol.hpp"

namespace nscheme {

class SymbolTable {
public:
    Symbol intern(const std::string& name) {
        const auto pair = symbols_.insert(name);
        return Symbol(&*pair.first);
    }

private:
    std::unordered_set<std::string> symbols_;
};

}
