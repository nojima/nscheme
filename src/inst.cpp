#include "inst.hpp"
#include "context.hpp"
#include "object.hpp"


namespace nscheme {


void LabelInst::exec(Context* ctx) {
    ctx->ip++;
}


void LoadNamedVariableInst::exec(Context* ctx) {
    auto it = ctx->named_variables.find(name_);
    if (it != ctx->named_variables.end()) {
        ctx->value_stack.push_back(it->second);
        ctx->ip++;
        return;
    }
    throw NameError("Undefined variable: " + name_.toString());
}


void LoadIndexedVariableInst::exec(Context* ctx) {
    Frame* frame = ctx->frame_stack.back();
    for (size_t i = 0; i < frame_index_; ++i)
        frame = frame->getParent();
    Value v = frame->getVariables()[variable_index_];
    ctx->value_stack.push_back(v);
    ctx->ip++;
}


void LoadLiteralInst::exec(Context* ctx) {
    ctx->value_stack.push_back(value_);
    ctx->ip++;
}


void LoadClosureInst::exec(Context* ctx) {
    Frame* frame = ctx->frame_stack.back();
    ClosureObject* closure = ctx->allocator->make<ClosureObject>(label_, frame, arg_size_, frame_size_);
    ctx->value_stack.push_back(Value::fromPointer(closure));
    ctx->ip++;
    if (ctx->allocator->needGc())
        ctx->allocator->gc(ctx);
}


void ApplyInst::exec(Context* ctx) {
    Value v = ctx->value_stack.back();
    ctx->value_stack.pop_back();
    if (v.isPointer()) {
        if (auto closure = dynamic_cast<ClosureObject*>(v.asPointer())) {
            if (closure->getArgSize() != n_args_)
                throw std::runtime_error("invalid number of arguments");

            std::vector<Value> args(closure->getFrameSize(), Value::Undefined);
            for (size_t i = 0; i < n_args_; ++i) {
                args[n_args_ - i - 1] = ctx->value_stack.back();
                ctx->value_stack.pop_back();
            }
            Frame* frame = ctx->allocator->make<Frame>(closure->getFrame(), args);
            if (tail_) {
                ctx->frame_stack.pop_back();
                ctx->frame_stack.push_back(frame);
            } else {
                ctx->control_stack.push_back(ctx->ip + 1);
                ctx->frame_stack.push_back(frame);
            }
            ctx->ip = closure->getLabel()->getLocation();

            if (ctx->allocator->needGc())
                ctx->allocator->gc(ctx);
            return;
        }
        if (auto cfunction = dynamic_cast<CFunctionObject*>(v.asPointer())) {
            cfunction->call(ctx, n_args_);
            ctx->ip++;
            return;
        }
        if (auto continuation = dynamic_cast<ContinuationObject*>(v.asPointer())) {
            std::vector<Value> value_stack = continuation->getValueStack();
            value_stack.insert(value_stack.end(), ctx->value_stack.end() - n_args_,
                               ctx->value_stack.end());
            ctx->value_stack = std::move(value_stack);
            ctx->control_stack = continuation->getControlStack();
            ctx->frame_stack = continuation->getFrameStack();
            ctx->ip = continuation->getInstrunctionPointer();
            return;
        }
    }
    throw TypeError("This object cannot be called.");
}


void NamedAssignInst::exec(Context* ctx) {
    auto it = ctx->named_variables.find(name_);
    if (it != ctx->named_variables.end()) {
        it->second = ctx->value_stack.back();
        ctx->value_stack.pop_back();
        ctx->value_stack.push_back(Value::Nil);
        ctx->ip++;
        return;
    }
    throw NameError("Undefined variable: " + name_.toString());
}


void IndexedAssignInst::exec(Context* ctx) {
    Frame* frame = ctx->frame_stack.back();
    for (size_t i = 0; i < frame_index_; ++i)
        frame = frame->getParent();
    frame->getVariables()[variable_index_] = ctx->value_stack.back();
    ctx->value_stack.pop_back();
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


void JumpInst::exec(Context* ctx) {
    ctx->ip = label_->getLocation();
}


void JumpIfInst::exec(Context* ctx) {
    Value value = ctx->value_stack.back();
    ctx->value_stack.pop_back();
    if (value.asBoolean()) {
        ctx->ip = label_->getLocation();
    } else {
        ctx->ip++;
    }
}


void QuitInst::exec(Context*) {
    throw Quit();
}


}   // namespace nscheme
