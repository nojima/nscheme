#include <cstdio>
#include <stdexcept>
#include "analyzer.hpp"
#include "parser.hpp"
#include "reader.hpp"
#include "scanner.hpp"
#include "symbol.hpp"
#include "symbol_table.hpp"
#include "value.hpp"
#include "code.hpp"
#include "inst.hpp"
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
        std::printf("     Datum: %s\n", value.toString().c_str());
        Analyzer analyzer(&symbol_table, &allocator, &source_map);
        Node* node = analyzer.analyze(value);
        std::printf("Expression: %s\n", node->toString().c_str());
        std::puts("==== Inst ====");
        Code code;
        node->codegen(code);
        code.main.push_back(new ReturnInst());
        for (Inst* inst: code.main)
            std::printf("%s\n", inst->toString().c_str());
        for (Inst* inst: code.sub)
            std::printf("%s\n", inst->toString().c_str());
        for (Inst* inst: code.main)
            delete inst;
        for (Inst* inst: code.sub)
            delete inst;
    } catch (const std::runtime_error& e) {
        std::fprintf(stderr, "%s\n", e.what());
        return 1;
    }
    return 0;
}
