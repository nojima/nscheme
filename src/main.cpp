#include <cstdio>
#include <stdexcept>
#include "reader.hpp"
#include "symbol.hpp"
#include "symbol_table.hpp"
#include "scanner.hpp"
#include "parser.hpp"
#include "value.hpp"
using namespace nscheme;

int main() {
    SymbolTable symbol_table;
    Symbol filename = symbol_table.intern("stdin");
    Allocator allocator;
    SourceMap source_map;
    try {
        FileReader reader(stdin, filename);
        Scanner scanner(&reader, &symbol_table);
        Parser parser(&scanner, &symbol_table, &allocator, &source_map);
        Value value = parser.parse();
        std::printf("%s\n", value.toString().c_str());
    } catch (const std::runtime_error& e) {
        std::fprintf(stderr, "%s\n", e.what());
        return 1;
    }
    return 0;
}
