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
#include "context.hpp"
using namespace nscheme;

std::vector<Inst*> codegen(Node* node) {
    Code code;
    node->codegen(code);
    code.main.push_back(new QuitInst());
    code.main.insert(code.main.end(), code.sub.begin(), code.sub.end());

    return std::move(code.main);
}

void resolveLabels(std::vector<Inst*>& code) {
    for (size_t i = 0; i < code.size(); ++i) {
        if (auto label = dynamic_cast<LabelInst*>(code[i])) {
            label->setLocation(&code[i]);
        }
    }
}

void plus(Context* ctx, size_t n_args) {
    int64_t sum = 0;
    for (size_t i = 0; i < n_args; ++i) {
        Value v = ctx->value_stack.back();
        ctx->value_stack.pop_back();
        if (!v.isInteger())
            throw TypeError("Argument of '+' must be an integer.");
        sum += v.asInteger();
    }
    ctx->value_stack.push_back(Value::fromInteger(sum));
}

void eq(Context* ctx, size_t n_args) {
    if (n_args != 2)
        throw std::runtime_error("=: Invalid number of arguments.");
    Value v1 = ctx->value_stack.back();
    ctx->value_stack.pop_back();
    Value v2 = ctx->value_stack.back();
    ctx->value_stack.pop_back();
    if (v1 == v2)
        ctx->value_stack.push_back(Value::True);
    else
        ctx->value_stack.push_back(Value::False);
}

void callcc(Context* ctx, size_t n_args) {
    if (n_args != 1)
        throw std::runtime_error("call/cc: Invalid number of arguments.");
    Value callable = ctx->value_stack.back();
    ctx->value_stack.pop_back();

    ContinuationObject* continuation = ctx->allocator->make<ContinuationObject>(
        ctx->ip + 1, ctx->value_stack, ctx->control_stack, ctx->frame_stack);
    ctx->value_stack.push_back(Value::fromPointer(continuation));
    ctx->value_stack.push_back(callable);
    ApplyInst(1).exec(ctx);
}

void run(std::vector<Inst*>& code, Allocator* allocator, SymbolTable* symbol_table) {
    Context ctx;
    ctx.ip = &code[0];
    ctx.allocator = allocator;

    std::unordered_map<Symbol, Value> variables;
    variables.insert(std::make_pair(
        symbol_table->intern("+"),
        Value::fromPointer(allocator->make<CFunctionObject>(plus, "+"))));
    variables.insert(std::make_pair(
        symbol_table->intern("eq?"),
        Value::fromPointer(allocator->make<CFunctionObject>(eq, "eq?"))));
    auto callcc_f = allocator->make<CFunctionObject>(callcc, "call-with-current-continuation");
    variables.insert(std::make_pair(
        symbol_table->intern("call-with-current-continuation"),
        Value::fromPointer(callcc_f)));
    variables.insert(std::make_pair(
        symbol_table->intern("call/cc"),
        Value::fromPointer(callcc_f)));
    Frame* frame = allocator->make<Frame>(nullptr, variables);
    ctx.frame_stack.push_back(frame);

    try {
        for (;;) {
            std::puts("====================================================");
            std::printf("Inst: %s\n", (*ctx.ip)->toString().c_str());

            (*ctx.ip)->exec(&ctx);

            std::printf("ValueStack:");
            for (auto it = ctx.value_stack.begin(); it != ctx.value_stack.end(); ++it)
                std::printf(" %s", it->toString().c_str());
            std::puts("");
        }
    } catch (Quit&) {
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

        run(code, &allocator, &symbol_table);

        for (Inst* inst: code)
            delete inst;
    } catch (const std::runtime_error& e) {
        std::fprintf(stderr, "%s\n", e.what());
        return 1;
    }
    return 0;
}
