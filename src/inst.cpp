#include "inst.hpp"
#include "object.hpp"

namespace nscheme {

void LabelInst::exec(Context* ctx) {
    ctx->ip++;
}

void LoadVariableInst::exec(Context* ctx) {
    Frame* frame = ctx->frame_stack.back();
    while (frame != nullptr) {
        auto it = frame->getVariables().find(name_);
        if (it != frame->getVariables().end()) {
            ctx->value_stack.push_back(it->second);
            ctx->ip++;
            return;
        }
        frame = frame->getParent();
    }
    throw NameError("Undefined variable: " + name_.toString());
}

void LoadLiteralInst::exec(Context* ctx) {
    ctx->value_stack.push_back(value_);
    ctx->ip++;
}

void LoadClosureInst::exec(Context* ctx) {
    Frame* frame = ctx->frame_stack.back();
    ClosureObject* closure = ctx->allocator->make<ClosureObject>(label_, frame, args_);
    ctx->value_stack.push_back(Value::fromPointer(closure));
    ctx->ip++;
}

void ApplyInst::exec(Context* ctx) {
    Value v = ctx->value_stack.back();
    ctx->value_stack.pop_back();
    if (v.isPointer()) {
        if (auto closure = dynamic_cast<ClosureObject*>(v.asPointer())) {
            std::unordered_map<Symbol, Value> args;
            for (size_t i = 0; i < n_args_; ++i) {
                Symbol name = closure->getArgNames()[n_args_ - i - 1];
                args.insert(std::make_pair(name, ctx->value_stack.back()));
                ctx->value_stack.pop_back();
            }
            Frame* frame = ctx->allocator->make<Frame>(closure->getFrame(), args);
            ctx->control_stack.push_back(ctx->ip + 1);
            ctx->frame_stack.push_back(frame);
            ctx->ip = closure->getLabel()->getLocation();
            return;
        }
        if (auto cfunction = dynamic_cast<CFunctionObject*>(v.asPointer())) {
            cfunction->call(ctx, n_args_);
            ctx->ip++;
            return;
        }
    }
    throw TypeError("This object cannot be called.");
}

void AssignInst::exec(Context* ctx) {
    Frame* frame = ctx->frame_stack.back();
    Value value = ctx->value_stack.back();
    ctx->value_stack.pop_back();
    frame->getVariables().insert(std::make_pair(name_, value));
    ctx->value_stack.push_back(Value::Nil);
    ctx->ip++;
}

void ReturnInst::exec(Context* ctx) {
    ctx->frame_stack.pop_back();
    ctx->ip = ctx->control_stack.back();
    ctx->control_stack.pop_back();
}

void DiscardInst::exec(Context* ctx) {
    ctx->value_stack.pop_back();
    ctx->ip++;
}

void BranchInst::exec(Context* ctx) {
    Value value = ctx->value_stack.back();
    ctx->value_stack.pop_back();
    ctx->control_stack.push_back(ctx->ip + 1);
    if (value.asBoolean()) {
        ctx->ip = then_label_->getLocation();
    } else {
        ctx->ip = else_label_->getLocation();
    }
}

void BranchReturnInst::exec(Context* ctx) {
    ctx->ip = ctx->control_stack.back();
    ctx->control_stack.pop_back();
}

void QuitInst::exec(Context*) {
    throw Quit();
}

}   // namespace nscheme
