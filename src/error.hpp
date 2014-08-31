#pragma once

#include <string>
#include <stdexcept>

#include "position.hpp"

namespace nscheme {

class BasicError: public std::runtime_error {
public:
    BasicError(const Position& position, const std::string& message):
        std::runtime_error(position.ToString() + ": " + message),
        position_(position), message_(message) {}

    virtual ~BasicError() {}

    const Position& Pos() const noexcept {
        return position_;
    }

    const std::string& Message() const noexcept {
        return message_;
    }

private:
    Position position_;
    std::string message_;
};

}   // namespace nscheme
