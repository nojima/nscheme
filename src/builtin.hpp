#include <unordered_map>
#include "allocator.hpp"
#include "symbol.hpp"
#include "symbol_table.hpp"
#include "value.hpp"


namespace nscheme {


void registerBuiltinFunctions(std::unordered_map<Symbol, Value>* variables,
                              Allocator* allocator,
                              SymbolTable* symbol_table);


}   // namespace nscheme
