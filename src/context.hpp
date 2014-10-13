#pragma once

#include <vector>

namespace nscheme {

class Inst;
struct Frame;

struct Context {
    Inst** ip;
    std::vector<Value> value_stack;
    std::vector<Inst**> control_stack;
    std::vector<Frame*> frame_stack;
};

}   // namespace nscheme
