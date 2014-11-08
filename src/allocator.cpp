#include "allocator.hpp"
#include "context.hpp"


namespace nscheme {


void Allocator::gc(Context* ctx)
{
    // std::printf("GC started: size=%zd, limit=%zd\n", size_, limit_);

    for (Object* obj : objects_)
        obj->resetMark();

    for (Value v : ctx->value_stack) {
        if (v.isPointer())
            v.asPointer()->mark();
    }
    for (Frame* f : ctx->frame_stack) {
        f->mark();
    }
    for (Value v : ctx->literals) {
        if (v.isPointer())
            v.asPointer()->mark();
    }

    auto pred = [this](const Object* obj) {
        if (!obj->isMarked()) {
            // std::printf("  GC collect: size=%zd\n", obj->size());
            size_ -= obj->size();
            delete obj;
            return true;
        }
        return false;
    };
    objects_.remove_if(pred);

    while (size_ > limit_)
        limit_ *= 2;

    // std::printf("GC end: size=%zd, limit=%zd\n", size_, limit_);
}


} // namespace nscheme
