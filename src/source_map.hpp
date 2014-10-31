#pragma once

#include <unordered_map>
#include <vector>
#include "object.hpp"
#include "position.hpp"


namespace nscheme {

using SourceMap = std::unordered_map<Object*, Position>;

}    // namespace nscheme
