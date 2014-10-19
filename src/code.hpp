#pragma once

#include <vector>

#include "inst.hpp"

namespace nscheme {

struct Code {
    std::vector<Inst*> main;
    std::vector<Inst*> sub;
};

}   // namespace nscheme
