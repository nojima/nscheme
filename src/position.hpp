#pragma once

#include <string>
#include "symbol.hpp"


namespace nscheme {


// ソースコード上の位置を表すクラス
class Position {
public:
    Position(Symbol filename, size_t line, size_t column)
        : filename_(filename)
        , line_(line)
        , column_(column)
    {
    }

    Symbol getFileName() const noexcept { return filename_; }

    size_t getLine() const noexcept { return line_; }

    size_t getColumn() const noexcept { return column_; }

    std::string toString() const
    {
        return filename_.toString() + ":" + std::to_string(line_) + ":" + std::to_string(column_);
    }

private:
    Symbol filename_;
    size_t line_;
    size_t column_;
};


} // namespace nscheme
