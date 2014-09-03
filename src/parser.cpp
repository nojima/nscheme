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
        obj = GetBooleanObject(true);
        scanner_->Next();
        return obj;
    case TokenType::kFalse:
        obj = GetBooleanObject(false);
        scanner_->Next();
        return obj;
    case TokenType::kInteger:
        obj = GetIntegerObject(token.Integer());
        scanner_->Next();
        return obj;
    case TokenType::kCharacter:
        obj = GetCharacterObject(token.Character());
        scanner_->Next();
        return obj;
    case TokenType::kString:
        obj = New<StringObject>(token.String());
        scanner_->Next();
        return obj;
    case TokenType::kIdentifier:
        obj = GetSymbolObject(token.Identifier());
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
    Object* first = GetNilObject();
    Object* last = GetNilObject();
    while (scanner_->Get().Type() != TokenType::kEof &&
           scanner_->Get().Type() != TokenType::kCloseParen) {
        if (IsNil(first)) {
            first = last = New<PairObject>(ParseDataum(), GetNilObject());
        } else if (scanner_->Get().Type() == TokenType::kPeriod) {
            scanner_->Next();   // skip '.'
            static_cast<PairObject*>(last)->SetCdr(ParseDataum());
        } else {
            PairObject* obj = New<PairObject>(ParseDataum(), nullptr);
            static_cast<PairObject*>(last)->SetCdr(obj);
            last = obj;
        }
    }
    if (scanner_->Get().Type() != TokenType::kCloseParen) {
        throw ParseError(scanner_->Get().Pos(), "expected ')'");
    }
    scanner_->Next();
    if (first == nullptr) {
        return GetNilObject();
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
    Object* p1 = New<PairObject>(obj, GetNilObject());
    Object* sym = GetSymbolObject(table_->Get("quote"));
    Object* p2 = New<PairObject>(sym, p1);
    return p2;
}

}   // namespace nscheme
