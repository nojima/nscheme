#include "parser.hpp"

#include <cassert>

#include "token.hpp"

namespace nscheme {

ObjectRef Parser::Parse() {
    scanner_->Next();
    return ParseDataum();
}

ObjectRef Parser::ParseDataum() {
    ObjectRef obj;
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
        obj = ref_cast(New<StringObject>(token.String()));
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

ObjectRef Parser::ParseList() {
    scanner_->Next();   // skip '('
    ObjectRef first = GetNilObject();
    ObjectRef last = GetNilObject();
    while (scanner_->Get().Type() != TokenType::kEof &&
           scanner_->Get().Type() != TokenType::kCloseParen) {
        if (IsNil(first)) {
            first = last = ref_cast(New<PairObject>(ParseDataum(), GetNilObject()));
        } else if (scanner_->Get().Type() == TokenType::kPeriod) {
            scanner_->Next();   // skip '.'
            reinterpret_cast<PairObject*>(last)->SetCdr(ParseDataum());
        } else {
            ObjectRef obj = ref_cast(New<PairObject>(ParseDataum(), GetNilObject()));
            reinterpret_cast<PairObject*>(last)->SetCdr(obj);
            last = obj;
        }
    }
    if (scanner_->Get().Type() != TokenType::kCloseParen) {
        throw ParseError(scanner_->Get().Pos(), "expected ')'");
    }
    scanner_->Next();
    return first;
}

ObjectRef Parser::ParseVector() {
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
    return ref_cast(obj);
}

ObjectRef Parser::ParseQuote() {
    scanner_->Next();   // skip quote char
    ObjectRef obj = ParseDataum();
    ObjectRef p1 = ref_cast(New<PairObject>(obj, GetNilObject()));
    ObjectRef sym = GetSymbolObject(table_->Get("quote"));
    ObjectRef p2 = ref_cast(New<PairObject>(sym, p1));
    return p2;
}

}   // namespace nscheme
