#pragma once

#include <vector>
#include <memory>
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

    // TODO: implement mark & sweep GC

private:
    std::vector<Object*> objects_;
    size_t memory_used_ = 0;

    template <typename T, typename ...Args>
    friend T* NewObject(ObjectList* list, Args&& ...args);
};

template <typename T, typename ...Args>
inline T* NewObject(ObjectList* list, Args&& ...args) {
    T* ptr = new T(std::forward<Args>(args)...);
    list->objects_.push_back(ptr);
    list->memory_used_ += sizeof(T);
    return ptr;
}

}   // namespace nscheme
