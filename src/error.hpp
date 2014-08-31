#pragma once

#include <string>

#include "position.hpp"

namespace nscheme {

class BasicError {
public:
    BasicError(const Position& position, const std::string& message):
        position_(position), message_(message) {}

    virtual ~BasicError() {}

    const Position& Pos() const noexcept {
        return position_;
    }

    const std::string& Message() const noexcept {
        return message_;
    }

    std::string ToString() const {
        return position_.ToString() + ": " + message_;
    }

private:
    Position position_;
    std::string message_;
};

}   // namespace nscheme
