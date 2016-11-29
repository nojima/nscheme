#include "builtin.hpp"
#include "context.hpp"
#include "inst.hpp"
#include "object.hpp"


namespace {

using namespace nscheme;


void sum(Context* ctx, size_t n_args)
{
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


void prod(Context* ctx, size_t n_args)
{
    int64_t prod = 1;
    for (size_t i = 0; i < n_args; ++i) {
        Value v = ctx->value_stack.back();
        ctx->value_stack.pop_back();
        if (!v.isInteger())
            throw TypeError("Argument of '*' must be an integer.");
        prod *= v.asInteger();
    }
    ctx->value_stack.push_back(Value::fromInteger(prod));
}


void callcc(Context* ctx, size_t n_args)
{
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


void registerFunction1(std::unordered_map<Symbol, Value>* variables, Allocator* allocator,
                       SymbolTable* symbol_table, const std::string& name,
                       const std::function<Value(Context*, Value)>& func)
{
    Symbol name_symbol = symbol_table->intern(name);
    auto f = [func, name_symbol](Context* ctx, size_t n_args) {
        if (n_args != 1) {
            throw std::runtime_error(name_symbol.toString() + ": Invalid number of arguments.");
        }
        Value v1 = ctx->value_stack.back();
        ctx->value_stack.pop_back();
        ctx->value_stack.push_back(func(ctx, v1));
    };
    Value v = Value::fromPointer(allocator->make<CFunctionObject>(f, name));
    variables->insert(std::make_pair(name_symbol, v));
}


void registerFunction2(std::unordered_map<Symbol, Value>* variables, Allocator* allocator,
                       SymbolTable* symbol_table, const std::string& name,
                       const std::function<Value(Context*, Value, Value)>& func)
{
    Symbol name_symbol = symbol_table->intern(name);
    auto f = [func, name_symbol](Context* ctx, size_t n_args) {
        if (n_args != 2) {
            throw std::runtime_error(name_symbol.toString() + ": Invalid number of arguments.");
        }
        Value v1 = ctx->value_stack.back();
        ctx->value_stack.pop_back();
        Value v2 = ctx->value_stack.back();
        ctx->value_stack.pop_back();
        ctx->value_stack.push_back(func(ctx, v2, v1));
    };
    Value v = Value::fromPointer(allocator->make<CFunctionObject>(f, name));
    variables->insert(std::make_pair(name_symbol, v));
}


} // namespace


namespace nscheme {


void registerBuiltinFunctions(std::unordered_map<Symbol, Value>* variables, Allocator* allocator,
                              SymbolTable* symbol_table)
{

    registerFunction1(variables, allocator, symbol_table, "not",
                      [](Context*, Value obj) { return Value::fromBoolean(!obj.asBoolean()); });

    variables->insert(std::make_pair(
        symbol_table->intern("+"), Value::fromPointer(allocator->make<CFunctionObject>(sum, "+"))));

    variables->insert(
        std::make_pair(symbol_table->intern("*"),
                       Value::fromPointer(allocator->make<CFunctionObject>(prod, "*"))));

    registerFunction2(variables, allocator, symbol_table, "-", [](Context*, Value a, Value b) {
        if (!a.isInteger() || !b.isInteger())
            throw TypeError("-: arguments must be integers");
        return Value::fromInteger(a.asInteger() - b.asInteger());
    });

    registerFunction2(variables, allocator, symbol_table, "/", [](Context*, Value a, Value b) {
        if (!a.isInteger() || !b.isInteger())
            throw TypeError("-: arguments must be integers");
        if (b.asInteger() == 0)
            throw std::runtime_error("/: divide by zero");
        return Value::fromInteger(a.asInteger() / b.asInteger());
    });

    registerFunction2(variables, allocator, symbol_table, "=", [](Context*, Value a, Value b) {
        if (!a.isInteger() || !b.isInteger())
            throw TypeError("=: arguments must be integers");
        return Value::fromBoolean(a.asInteger() == b.asInteger());
    });

    registerFunction2(variables, allocator, symbol_table, "<", [](Context*, Value a, Value b) {
        if (!a.isInteger() || !b.isInteger())
            throw TypeError("<: arguments must be integers");
        return Value::fromBoolean(a.asInteger() < b.asInteger());
    });

    registerFunction2(variables, allocator, symbol_table, ">", [](Context*, Value a, Value b) {
        if (!a.isInteger() || !b.isInteger())
            throw TypeError(">: arguments must be integers");
        return Value::fromBoolean(a.asInteger() > b.asInteger());
    });

    registerFunction2(variables, allocator, symbol_table, "<=", [](Context*, Value a, Value b) {
        if (!a.isInteger() || !b.isInteger())
            throw TypeError("<=: arguments must be integers");
        return Value::fromBoolean(a.asInteger() <= b.asInteger());
    });

    registerFunction2(variables, allocator, symbol_table, ">=", [](Context*, Value a, Value b) {
        if (!a.isInteger() || !b.isInteger())
            throw TypeError(">=: arguments must be integers");
        return Value::fromBoolean(a.asInteger() >= b.asInteger());
    });

    registerFunction2(variables, allocator, symbol_table, "eq?",
                      [](Context*, Value a, Value b) { return Value::fromBoolean(a == b); });

    registerFunction1(variables, allocator, symbol_table, "pair?",
                      [](Context*, Value obj) { return Value::fromBoolean(isPair(obj)); });

    registerFunction2(variables, allocator, symbol_table, "cons",
                      [](Context* ctx, Value a, Value b) {
        return Value::fromPointer(ctx->allocator->make<PairObject>(a, b));
    });

    registerFunction1(variables, allocator, symbol_table, "car", [](Context*, Value pair) {
        if (!isPair(pair))
            throw TypeError("car: 1st argument must be a pair");
        return static_cast<PairObject*>(pair.asPointer())->getCar();
    });

    registerFunction1(variables, allocator, symbol_table, "cdr", [](Context*, Value pair) {
        if (!isPair(pair))
            throw TypeError("cdr: 1st argument must be a pair");
        return static_cast<PairObject*>(pair.asPointer())->getCdr();
    });

    registerFunction1(variables, allocator, symbol_table, "null?",
                      [](Context*, Value obj) { return Value::fromBoolean(obj == Value::Nil); });

    registerFunction1(variables, allocator, symbol_table, "print", [](Context*, Value obj) {
        std::printf("%s\n", obj.toString().c_str());
        return Value::Nil;
    });

    auto callcc_f = allocator->make<CFunctionObject>(callcc, "call-with-current-continuation");
    variables->insert(std::make_pair(symbol_table->intern("call-with-current-continuation"),
                                     Value::fromPointer(callcc_f)));
    variables->insert(
        std::make_pair(symbol_table->intern("call/cc"), Value::fromPointer(callcc_f)));
}


} // namespace nscheme
