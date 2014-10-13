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

std::vector<Inst*> codegen(Node* node) {
    Code code;
    node->codegen(code);
    code.main.push_back(new ReturnInst());
    code.main.insert(code.main.end(), code.sub.begin(), code.sub.end());

    return std::move(code.main);
}

void resolveLabels(std::vector<Inst*>& code) {
    for (size_t i = 0; i < code.size(); ++i) {
        if (auto label = dynamic_cast<LabelInst*>(code[i])) {
            label->setLocation(reinterpret_cast<LabelInst**>(&code[i]));
        }
    }
}

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
        std::vector<Inst*> code = codegen(node);
        resolveLabels(code);
        for (Inst* inst: code)
            std::printf("%s\n", inst->toString().c_str());
        for (Inst* inst: code)
            delete inst;
    } catch (const std::runtime_error& e) {
        std::fprintf(stderr, "%s\n", e.what());
        return 1;
    }
    return 0;
}
