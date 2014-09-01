#include "parser.hpp"

#include <cassert>

#include "token.hpp"

namespace nscheme {

Object* Parser::Parse() {
    scanner_->Next();
}

Object* Parser::ParseDataum() {
    const Token& token = scanner_->Get();
    switch (token.Type()) {
    case TokenType::kTrue:
        return object_list_->Create<BooleanObject>(true);
    case TokenType::kFalse:
        return object_list_->Create<BooleanObject>(false);
    case TokenType::kInteger:
        return object_list_->Create<IntegerObject>(token.Integer());
    case TokenType::kCharacter:
        return object_list_->Create<CharacterObject>(token.Character());
    case TokenType::kString:
        return object_list_->Create<StringObject>(token.String());
    case TokenType::kIdentifier:
        return object_list_->Create<SymbolObject>(token.Identifier());
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
    Object* first = nullptr;
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
    if (first == nullptr) {
        return object_list_->Create<NilObject>();
    } else {
        return first;
    }
}

}   // namespace nscheme
