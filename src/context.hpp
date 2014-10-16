#pragma once

#include <vector>
#include "allocator.hpp"

namespace nscheme {

class Inst;
class Frame;

struct Context {
    Inst** ip;
    std::vector<Value> value_stack;
    std::vector<Inst**> control_stack;
    std::vector<Frame*> frame_stack;
    Allocator* allocator;
};

}   // namespace nscheme
