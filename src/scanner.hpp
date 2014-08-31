#pragma once

#include <iterator>
#include <string>

#include "token.hpp"

namespace nscheme {

class Scanner {
public:
    Scanner(const std::istreambuf_iterator<char>& stream,
            const std::istreambuf_iterator<char>& last,
            const std::string& filename):
        stream_(stream), last_(last), filename_(filename) {}

    const Token& Get() const noexcept {
        return token_;
    }

    void Next();

private:
    std::istreambuf_iterator<char> stream_;
    std::istreambuf_iterator<char> last_;
    const std::string& filename_;
    size_t line_ = 1;
    size_t column_ = 1;
    Token token_ = Token::CreateEof();
};

}   // namespace nscheme
