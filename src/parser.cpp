#include "parser.hpp"

#include <cassert>

#include "token.hpp"

namespace nscheme {

Object* Parser::Parse() {
    scanner_->Next();
    return ParseDataum();
}

Object* Parser::ParseDataum() {
    Object* obj;
    const Token& token = scanner_->Get();
    switch (token.Type()) {
    case TokenType::kTrue:
        obj = New<BooleanObject>(true);
        scanner_->Next();
        return obj;
    case TokenType::kFalse:
        obj = New<BooleanObject>(false);
        scanner_->Next();
        return obj;
    case TokenType::kInteger:
        obj = New<IntegerObject>(token.Integer());
        scanner_->Next();
        return obj;
    case TokenType::kCharacter:
        obj = New<CharacterObject>(token.Character());
        scanner_->Next();
        return obj;
    case TokenType::kString:
        obj = New<StringObject>(token.String());
        scanner_->Next();
        return obj;
    case TokenType::kIdentifier:
        obj = New<SymbolObject>(token.Identifier());
        scanner_->Next();
        return obj;
    case TokenType::kOpenParen:
        return ParseList();
    case TokenType::kOpenVector:
        return ParseVector();
    case TokenType::kQuote:
        return ParseQuote();
    default:
        // TODO: implement
        assert(0);
    }
}

Object* Parser::ParseList() {
    scanner_->Next();   // skip '('
    PairObject* first = nullptr;
    PairObject* last = nullptr;
    while (scanner_->Get().Type() != TokenType::kEof &&
           scanner_->Get().Type() != TokenType::kCloseParen) {
        if (first == nullptr) {
            first = last = New<PairObject>(ParseDataum(), nullptr);
        } else if (scanner_->Get().Type() == TokenType::kPeriod) {
            scanner_->Next();   // skip '.'
            last->SetCdr(ParseDataum());
        } else {
            PairObject* obj = New<PairObject>(ParseDataum(), nullptr);
            last->SetCdr(obj);
            last = obj;
        }
    }
    if (last != nullptr & last->Cdr() == nullptr) {
        last->SetCdr(New<NilObject>());
    }
    if (scanner_->Get().Type() != TokenType::kCloseParen) {
        throw ParseError(scanner_->Get().Pos(), "expected ')'");
    }
    scanner_->Next();
    if (first == nullptr) {
        return New<NilObject>();
    } else {
        return first;
    }
}

Object* Parser::ParseVector() {
    scanner_->Next();   // skip '#('
    VectorObject* obj = New<VectorObject>();
    while (scanner_->Get().Type() != TokenType::kEof &&
           scanner_->Get().Type() != TokenType::kCloseParen) {
        obj->Add(ParseDataum());
    }
    if (scanner_->Get().Type() != TokenType::kCloseParen) {
        throw ParseError(scanner_->Get().Pos(), "expected ')'");
    }
    scanner_->Next();
    return obj;
}

Object* Parser::ParseQuote() {
    scanner_->Next();   // skip quote char
    Object* obj = ParseDataum();
    Object* p1 = New<PairObject>(obj, New<NilObject>());
    Object* sym = New<SymbolObject>(table_->Get("quote"));
    Object* p2 = New<PairObject>(sym, p1);
    return p2;
}

}   // namespace nscheme
