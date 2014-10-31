#include "reader.hpp"
#include <cassert>
#include "object.hpp"


namespace nscheme {


Value Reader::read() {
    return readDatum();
}


Value Reader::readDatum() {
    Value value = Value::Nil;

    switch (token_.getType()) {
    case TokenType::kTrue:
        token_ = scanner_->getToken();
        return Value::True;

    case TokenType::kFalse:
        token_ = scanner_->getToken();
        return Value::False;

    case TokenType::kInteger:
        value = Value::fromInteger(token_.getInteger());
        token_ = scanner_->getToken();
        return value;

    case TokenType::kReal:
        value = Value::fromPointer(
            allocator_->make<RealObject>(token_.getReal()));
        token_ = scanner_->getToken();
        return value;

    case TokenType::kCharacter:
        value = Value::fromCharacter(token_.getCharacter());
        token_ = scanner_->getToken();
        return value;

    case TokenType::kString:
        value = Value::fromPointer(
            allocator_->make<StringObject>(token_.getString()));
        token_ = scanner_->getToken();
        return value;

    case TokenType::kIdentifier:
        value = Value::fromSymbol(token_.getSymbol());
        token_ = scanner_->getToken();
        return value;

    case TokenType::kOpenParen:
        return readList();

    case TokenType::kSharpOpenParen:
        return readVector();

    case TokenType::kQuote: case TokenType::kBackQuote:
    case TokenType::kComma: case TokenType::kCommaAt:
        return readAbbr();

    default:
        throw ReadError(token_.getPosition(), "unexpected token");
    }
}


Value Reader::readList() {
    Position position = token_.getPosition();
    token_ = scanner_->getToken();
    PairObject* first = nullptr;
    PairObject* last = nullptr;
    while (token_.getType() != TokenType::kEof &&
           token_.getType() != TokenType::kCloseParen) {
        if (first == nullptr) {
            Position pos = token_.getPosition();
            first = last = allocator_->make<PairObject>(readDatum(), Value::Nil);
            source_map_->insert(std::make_pair(last, pos));
        } else if (token_.getType() == TokenType::kPeriod) {
            token_ = scanner_->getToken();      // skip '.'
            last->setCdr(readDatum());
            if (token_.getType() != TokenType::kCloseParen)
                throw ReadError(token_.getPosition(), "expected ')'");
            break;
        } else {
            Position pos = token_.getPosition();
            PairObject* p = allocator_->make<PairObject>(readDatum(), Value::Nil);
            last->setCdr(Value::fromPointer(p));
            last = p;
            source_map_->insert(std::make_pair(last, pos));
        }
    }
    if (token_.getType() != TokenType::kCloseParen)
        throw ReadError(position, "unclosed list");
    token_ = scanner_->getToken();
    return Value::fromPointer(first);
}


Value Reader::readVector() {
    Position position = token_.getPosition();
    token_ = scanner_->getToken();
    VectorObject* obj = allocator_->make<VectorObject>();
    while (token_.getType() != TokenType::kEof &&
           token_.getType() != TokenType::kCloseParen) {
        obj->add(readDatum());
    }
    if (token_.getType() != TokenType::kCloseParen)
        throw ReadError(position, "unclosed vector literal");
    token_ = scanner_->getToken();
    source_map_->insert(std::make_pair(obj, position));
    return Value::fromPointer(obj);
}


Value Reader::readAbbr() {
    Position position = token_.getPosition();
    TokenType type = token_.getType();
    const char* name;
    switch (type) {
    case TokenType::kQuote:     name = "quote"; break;
    case TokenType::kBackQuote: name = "quasiquote"; break;
    case TokenType::kComma:     name = "unquote"; break;
    case TokenType::kCommaAt:   name = "unqote-splicing"; break;
    default: assert(0);
    }
    Symbol symbol = symbol_table_->intern(name);
    token_ = scanner_->getToken();
    Value v = readDatum();
    PairObject* p1 = allocator_->make<PairObject>(v, Value::Nil);
    PairObject* p2 = allocator_->make<PairObject>(
        Value::fromSymbol(symbol), Value::fromPointer(p1));
    source_map_->insert(std::make_pair(p1, position));
    source_map_->insert(std::make_pair(p2, position));
    return Value::fromPointer(p2);
}


}   // namespace nscheme
