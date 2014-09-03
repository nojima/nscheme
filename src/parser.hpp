#pragma once

#include <stdexcept>
#include <unordered_map>
#include <string>

#include "object.hpp"
#include "object_list.hpp"
#include "scanner.hpp"

namespace nscheme {

class Parser {
public:
    explicit Parser(Scanner* scanner, SymbolTable* table, ObjectList* object_list):
        scanner_(scanner), table_(table), object_list_(object_list) {
        if (!scanner)
            throw std::logic_error("scanner must not be null");
    }

    ObjectRef Parse();

private:
    // shortcut function to reduce redundant code
    template <typename T, typename ...Args>
    T* New(Args&& ...args) {
        return NewObject<T>(object_list_, std::forward<Args>(args)...);
    }

    ObjectRef ParseDataum();
    ObjectRef ParseList();
    ObjectRef ParseVector();
    ObjectRef ParseQuote();

    Scanner* scanner_;
    std::unordered_map<std::int64_t, ObjectRef> label_map_;
    SymbolTable* table_;
    ObjectList* object_list_;
};

class ParseError: public BasicError {
public:
    ParseError(const Position& position, const std::string& message):
        BasicError(position, message) {}
};

}
