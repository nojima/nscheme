#include "translator.hpp"
#include "stream.hpp"
#include "scanner.hpp"
#include "reader.hpp"
#include "parser.hpp"
#include "symbol_table.hpp"
#include "source_map.hpp"
#include "gtest/gtest.h"
using namespace nscheme;


class SyntaxRuleTest : public ::testing::Test {
protected:
    Value read(const std::string s)
    {
        StringStream stream(s, symbol_table_.intern(s));
        Scanner scanner(&stream, &symbol_table_);
        Reader reader(&scanner, &symbol_table_, &allocator_, &source_map_);
        return reader.read();
    }

    SymbolTable symbol_table_;
    Allocator allocator_;
    SourceMap source_map_;
};


TEST_F(SyntaxRuleTest, SimpleSymbol)
{
    Value pattern = read("foo");
    Value tmpl = read("(set! foo 1)");
    std::vector<Symbol> literal;
    LocalNames names(nullptr);
    SyntaxRule rule(pattern, tmpl, literal, names, &allocator_, &symbol_table_);

    Value expr = read("my-symbol");
    std::unordered_map<Symbol, Value> mapping;
    bool r = rule.match(pattern, expr, mapping);
    EXPECT_TRUE(r);
    EXPECT_EQ(1, mapping.count(symbol_table_.intern("foo")));
}


TEST_F(SyntaxRuleTest, SimpleList)
{
    Value pattern = read("(_ foo bar baz)");
    Value tmpl = read("(set! foo bar)");
    std::vector<Symbol> literal;
    LocalNames names(nullptr);
    SyntaxRule rule(pattern, tmpl, literal, names, &allocator_, &symbol_table_);

    Value expr = read("(simple-macro my-symbol 100 #t)");
    std::unordered_map<Symbol, Value> mapping;
    bool r = rule.match(pattern, expr, mapping);
    EXPECT_TRUE(r);
    EXPECT_EQ(1, mapping.count(symbol_table_.intern("foo")));
    EXPECT_EQ(1, mapping.count(symbol_table_.intern("bar")));
    EXPECT_EQ(1, mapping.count(symbol_table_.intern("baz")));
}


TEST_F(SyntaxRuleTest, SimpleLiteral)
{
    Value pattern = read("(_ foo to bar)");
    Value tmpl = read("(set! foo bar)");
    std::vector<Symbol> literal = { symbol_table_.intern("to") };
    LocalNames names(nullptr);
    SyntaxRule rule(pattern, tmpl, literal, names, &allocator_, &symbol_table_);

    Value expr = read("(simple-macro my-symbol to 100)");
    std::unordered_map<Symbol, Value> mapping;
    bool r = rule.match(pattern, expr, mapping);
    EXPECT_TRUE(r);
    EXPECT_EQ(1, mapping.count(symbol_table_.intern("foo")));
    EXPECT_EQ(0, mapping.count(symbol_table_.intern("to")));
    EXPECT_EQ(1, mapping.count(symbol_table_.intern("bar")));
}
