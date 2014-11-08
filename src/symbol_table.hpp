#pragma once

#include <string>
#include <stdexcept>
#include <unordered_set>
#include "symbol.hpp"


namespace nscheme {


class SymbolTable {
public:
    Symbol intern(const std::string& name)
    {
        // unordered_set への insert は既存の要素へのポインタを
        // invalidate しないので安全
        const auto pair = symbols_.insert(name);
        return Symbol(&*pair.first);
    }

private:
    std::unordered_set<std::string> symbols_;
};


} // namespace nscheme
