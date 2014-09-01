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
        obj = object_list_->Create<BooleanObject>(true);
        scanner_->Next();
        return obj;
    case TokenType::kFalse:
        obj = object_list_->Create<BooleanObject>(false);
        scanner_->Next();
        return obj;
    case TokenType::kInteger:
        obj = object_list_->Create<IntegerObject>(token.Integer());
        scanner_->Next();
        return obj;
    case TokenType::kCharacter:
        obj = object_list_->Create<CharacterObject>(token.Character());
        scanner_->Next();
        return obj;
    case TokenType::kString:
        obj = object_list_->Create<StringObject>(token.String());
        scanner_->Next();
        return obj;
    case TokenType::kIdentifier:
        obj = object_list_->Create<SymbolObject>(token.Identifier());
        scanner_->Next();
        return obj;
    case TokenType::kOpenParen:
        return ParseList();
    case TokenType::kOpenVector:
        return ParseVector();
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
            first = last = object_list_->Create<PairObject>(ParseDataum(), nullptr);
        } else if (scanner_->Get().Type() == TokenType::kPeriod) {
            scanner_->Next();   // skip '.'
            last->SetCdr(ParseDataum());
        } else {
            PairObject* obj = object_list_->Create<PairObject>(ParseDataum(), nullptr);
            last->SetCdr(obj);
            last = obj;
        }
    }
    if (last != nullptr & last->Cdr() == nullptr) {
        last->SetCdr(object_list_->Create<NilObject>());
    }
    if (scanner_->Get().Type() != TokenType::kCloseParen) {
        throw ParseError(scanner_->Get().Pos(), "expected ')'");
    }
    scanner_->Next();
    if (first == nullptr) {
        return object_list_->Create<NilObject>();
    } else {
        return first;
    }
}

Object* Parser::ParseVector() {
    scanner_->Next();   // skip '#('
    VectorObject* obj = object_list_->Create<VectorObject>();
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

}   // namespace nscheme
