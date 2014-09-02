#pragma once

#include <vector>
#include <utility>

#include "object.hpp"

namespace nscheme {

class ObjectList {
public:
    ~ObjectList() {
        for (Object* p : objects_) {
            delete p;
        }
    }

    template<typename T, typename ...Args>
    T* Create(Args&& ...args) {
        T* ptr = new T(std::forward<Args>(args)...);
        objects_.push_back(ptr);
        memory_used_ += sizeof(T);
        return ptr;
    }

    // TODO: implement mark & sweep GC

private:
    std::vector<Object*> objects_;
    size_t memory_used_ = 0;
};

}   // namespace nscheme
