#include <algorithm>
#include <cstdio>
#include <stdexcept>
#include "argparse.hpp"
#include "builtin.hpp"
#include "code.hpp"
#include "context.hpp"
#include "inst.hpp"
#include "parser.hpp"
#include "reader.hpp"
#include "scanner.hpp"
#include "stream.hpp"
#include "symbol.hpp"
#include "symbol_table.hpp"
#include "value.hpp"
using namespace nscheme;


std::vector<Inst*> codegen(Node* node)
{
    Code code;
    node->codegen(code);
    code.main.push_back(new QuitInst());
    code.main.insert(code.main.end(), code.sub.begin(), code.sub.end());

    return std::move(code.main);
}


void resolveLabels(std::vector<Inst*>& code)
{
    for (size_t i = 0; i < code.size(); ++i) {
        if (auto label = dynamic_cast<LabelInst*>(code[i])) {
            label->setLocation(&code[i]);
        }
    }
}


bool optimizeOnePass(std::vector<Inst*>& code)
{
    bool changed = false;
    Inst** last = &code[0] + code.size();

    for (size_t i = 0; i < code.size(); ++i) {
        if (auto apply = dynamic_cast<ApplyInst*>(code[i])) {
            for (size_t j = i + 1; j < code.size(); ++j) {
                if (dynamic_cast<ReturnInst*>(code[j])) {
                    apply->setTail(true);
                    changed = true;
                    break;
                }
                if (dynamic_cast<LabelInst*>(code[j]) == nullptr)
                    break;
            }
        }
        else if (auto jump = dynamic_cast<JumpInst*>(code[i])) {
            Inst** ip = jump->getLabel()->getLocation();
            for (; ip != last; ip++) {
                if (dynamic_cast<ReturnInst*>(*ip)) {
                    delete code[i];
                    code[i] = new ReturnInst;
                    changed = true;
                    break;
                }
                if (auto jump2 = dynamic_cast<JumpInst*>(*ip)) {
                    delete code[i];
                    code[i] = new JumpInst(jump2->getLabel());
                    changed = true;
                    break;
                }
                if (dynamic_cast<LabelInst*>(*ip) == nullptr)
                    break;
            }
        }
    }

    return changed;
}


void optimize(std::vector<Inst*>& code)
{
    const int maxPass = 7;

    for (int i = 0; i < maxPass; ++i) {
        if (!optimizeOnePass(code))
            return;
    }
}


void printInst(Context& ctx)
{
    std::puts("====================================================");
    std::printf("Inst: %s\n", (*ctx.ip)->toString().c_str());
}


void printState(Context& ctx)
{
    std::printf("ValueStack:");
    for (auto it = ctx.value_stack.begin(); it != ctx.value_stack.end(); ++it)
        std::printf(" %s", it->toString().c_str());
    std::puts("");

    std::printf("Scope: ");
    for (auto f = ctx.frame_stack.back(); f != nullptr; f = f->getParent()) {
        if (f->getParent() == nullptr) {
            std::printf("{global}");
        }
        else {
            std::printf("{");
            auto& variables = f->getVariables();
            for (size_t i = 0; i < variables.size(); ++i) {
                if (i != 0)
                    std::printf(", ");
                std::printf("%zd: %s", i, variables[i].toString().c_str());
            }
            std::printf("}, ");
        }
    }
    std::puts("");
}


Context createContext(std::vector<Inst*>& code, Allocator* allocator,
                      std::unordered_map<Symbol, Value>& global_variables)
{
    Context ctx;

    ctx.ip = &code[0];
    ctx.allocator = allocator;
    for (Inst* inst : code) {
        if (auto literal = dynamic_cast<LoadLiteralInst*>(inst))
            ctx.literals.push_back(literal->getValue());
    }
    ctx.named_variables = global_variables;

    std::vector<std::pair<Symbol, Value>> tmp;
    tmp.reserve(global_variables.size());
    for (auto it : global_variables)
        tmp.push_back(it);
    std::sort(tmp.begin(), tmp.end(),
              [](const std::pair<Symbol, Value>& a, const std::pair<Symbol, Value>& b) {
        return a.first.toString() < b.first.toString();
    });
    std::vector<Value> variables;
    variables.reserve(tmp.size());
    for (auto it : tmp)
        variables.push_back(it.second);
    Frame* frame = allocator->make<Frame>(nullptr, variables);
    ctx.frame_stack.push_back(frame);

    return ctx;
}


int run(std::vector<Inst*>& code, Allocator* allocator,
        std::unordered_map<Symbol, Value>& global_variables, bool trace)
{

    Context ctx = createContext(code, allocator, global_variables);

    try {
        for (;;) {
            if (trace)
                printInst(ctx);

            (*ctx.ip)->exec(&ctx);

            if (trace)
                printState(ctx);
        }
    }
    catch (Quit&) {
    }
    catch (std::runtime_error& e) {
        std::printf("[ERROR] %s\n", e.what());
        return 1;
    }

    return 0;
}


void usage()
{
    puts("Usage: nscheme [--help] [--trace] [FILE]");
    puts("Options:");
    puts("  --help   show this message and exit");
    puts("  --trace  show internal state of the interpreter");
}


int main(int argc, char** argv)
{
    bool trace = false;
    std::string filename = "-";

    ArgumentParser argparser;
    argparser.addOption("trace", "t", "trace");
    argparser.addOption("help", "h", "help");
    argparser.addArgument("filename");

    try {
        auto args = argparser.parse(argc, argv);
        if (args.count("help")) {
            usage();
            return 1;
        }
        if (args.count("trace")) {
            trace = true;
        }
        if (args.count("filename")) {
            filename = args["filename"];
        }
    }
    catch (ArgumentParseError& e) {
        std::fprintf(stderr, "%s\n", e.what());
        usage();
        return 1;
    }

    SymbolTable symbol_table;
    Allocator allocator;
    SourceMap source_map;

    try {
        FILE* file = stdin;
        if (filename != "-") {
            file = fopen(filename.c_str(), "r");
            if (file == nullptr) {
                fprintf(stderr, "Failed to open file: %s\n", filename.c_str());
                return 1;
            }
        }

        FileStream stream(file, symbol_table.intern(filename));
        Scanner scanner(&stream, &symbol_table);
        Reader reader(&scanner, &symbol_table, &allocator, &source_map);
        Value value = reader.read();
        if (trace)
            std::printf("Datum: %s\n", value.toString().c_str());

        std::unordered_map<Symbol, Value> global_variables;
        registerBuiltinFunctions(&global_variables, &allocator, &symbol_table);

        Parser parser(&symbol_table, &source_map, global_variables);
        std::unique_ptr<Node> node(parser.parse(value));
        if (trace)
            std::printf("Expression: %s\n", node->toString().c_str());

        std::vector<Inst*> code = codegen(node.get());
        resolveLabels(code);
        optimize(code);

        if (trace) {
            std::puts("==== Inst ====");
            for (Inst* inst : code)
                std::printf("%s\n", inst->toString().c_str());
        }

        int rc = run(code, &allocator, global_variables, trace);

        for (Inst* inst : code)
            delete inst;

        return rc;
    }
    catch (const std::runtime_error& e) {
        std::fprintf(stderr, "%s\n", e.what());
        return 1;
    }
}
