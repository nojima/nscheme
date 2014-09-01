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
    explicit Parser(Scanner* scanner, ObjectList* object_list):
        scanner_(scanner), object_list_(object_list) {
        if (!scanner)
            throw std::logic_error("scanner must not be null");
    }

    Object* Parse();

private:
    Object* ParseDataum();
    Object* ParseList();
    //Object* ParseVector();

    Scanner* scanner_;
    std::unordered_map<std::int64_t, Object*> label_map_;
    ObjectList* object_list_;
};

class ParseError: public BasicError {
public:
    ParseError(const Position& position, const std::string& message):
        BasicError(position, message) {}
};

}
