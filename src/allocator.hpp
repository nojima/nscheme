#pragma once

#include <forward_list>
#include <utility>
#include "object.hpp"


namespace nscheme {


class Allocator {
public:
    // TODO: implement GC
    ~Allocator() {
        for (Object* p : objects_)
            delete p;
    }

    template <typename T, typename ...Args>
    T* make(Args&& ...args) {
        T* ptr = new T(std::forward<Args>(args)...);
        objects_.push_front(ptr);
        return ptr;
    }

private:
    std::forward_list<Object*> objects_;
};


}   // namespace nscheme
