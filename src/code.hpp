#pragma once

#include <vector>

#include "inst.hpp"

namespace nscheme {

struct Code {
    Code() {}
    Code(const std::vector<Inst*>& main, const std::vector<Inst*>& sub)
        : main(main), sub(sub) {}

    std::vector<Inst*> main;
    std::vector<Inst*> sub;
};

}   // namespace nscheme
