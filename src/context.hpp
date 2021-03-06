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
    std::vector<Value> literals;
    std::unordered_map<Symbol, Value> named_variables;
    Allocator* allocator;
};


} // namespace nscheme
