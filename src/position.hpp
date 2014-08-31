#pragma once

#include <string>

namespace nscheme {

class Position {
public:
    Position(const std::string& filename, size_t line, size_t column):
        filename_(filename), line_(line), column_(column) {}

    const std::string& FileName() const noexcept {
        return filename_;
    }

    size_t Line() const noexcept {
        return line_;
    }

    size_t Column() const noexcept {
        return column_;
    }

    std::string ToString() const {
        return filename_ + ":" + std::to_string(line_) + ":" + std::to_string(column_);
    }

private:
    const std::string& filename_;
    size_t line_;
    size_t column_;
};

}   // namespace nscheme
