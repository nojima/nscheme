#pragma once

#include <forward_list>
#include <utility>
#include "object.hpp"


namespace nscheme {

struct Context;


class Allocator {
public:
    ~Allocator() {
        for (Object* p: objects_)
            delete p;
    }

    template <typename T, typename ...Args>
    T* make(Args&& ...args) {
        T* ptr = new T(std::forward<Args>(args)...);
        objects_.push_front(ptr);
        size_ += sizeof(T);
        return ptr;
    }

    bool needGc() const {
        return size_ > limit_;
    }

    void gc(Context* ctx);

private:
    std::forward_list<Object*> objects_;
    size_t size_ = 0;
    size_t limit_ = 1024;
};


}   // namespace nscheme
