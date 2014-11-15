#pragma once

#include <cstdio>
#include "position.hpp"


namespace nscheme {


// ストリームや文字列から一文字ずつ文字を読み込むクラス
class Stream {
public:
    virtual ~Stream() {}

    // 次の一文字を返す。
    // 終端に達した時は EOF を返す。
    virtual int getChar() = 0;

    // 現在の場所を返す。
    virtual Position getPosition() const = 0;
};


// ファイルから文字を読み込むクラス
class FileStream : public Stream {
public:
    FileStream(FILE* file, Symbol filename)
        : file_(file)
        , filename_(filename)
    {
    }

    int getChar() override
    {
        int ch = fgetc(file_);
        if (ch == '\n') {
            ++line_;
            column_ = 1;
        }
        else {
            ++column_;
        }
        return ch;
    }

    Position getPosition() const override { return Position(filename_, line_, column_); }

private:
    FILE* file_;
    Symbol filename_;
    size_t line_ = 1;
    size_t column_ = 1;
};


// 文字列から文字を読み込むクラス
class StringStream : public Stream {
public:
    StringStream(const std::string& str, Symbol filename)
        : str_(str)
        , filename_(filename)
    {
    }

    int getChar() override
    {
        int ch = (index_ < str_.size()) ? str_[index_] : EOF;
        ++index_;
        if (ch == '\n') {
            ++line_;
            column_ = 1;
        }
        else {
            ++column_;
        }
        return ch;
    }

    Position getPosition() const override { return Position(filename_, line_, column_); }

private:
    std::string str_;
    Symbol filename_;
    size_t index_ = 0;
    size_t line_ = 1;
    size_t column_ = 1;
};


} // namespace nscheme
