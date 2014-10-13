#pragma once

#include <unordered_map>

#include "symbol.hpp"
#include "value.hpp"

namespace nscheme {

struct Frame {
    Frame* parent;
    std::unordered_map<Symbol, Value> variables;
};

}   // namespace nscheme
