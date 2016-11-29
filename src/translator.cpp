#include "translator.hpp"
#include <algorithm>


namespace nscheme {


bool SyntaxRule::match(Value pattern, Value expr, std::unordered_map<Symbol, Value>& mapping) const
{
    if (pattern.isSymbol()) {
        Symbol symbol = pattern.asSymbol();
        if (symbol == kwd_underscore_)
            return true;
        auto it = std::find(literals_.begin(), literals_.end(), symbol);
        if (it != literals_.end()) {
            // symbol is literal
            return expr.isSymbol() && symbol == expr.asSymbol();
        }
        // symbol is not literal
        mapping.insert(std::make_pair(symbol, expr));
        return true;
    }

    if (isPair(pattern)) {
        if (!isPair(expr))
            return false;

        ssize_t ellipsis_index = -1;
        ssize_t pattern_len = -1;
        bool is_proper = false;

        auto p = static_cast<PairObject*>(pattern.asPointer());
        for (size_t i = 0;; ++i) {
            Value car = p->getCar();
            if (car.isSymbol() && car.asSymbol() == kwd_ellipsis_) {
                if (ellipsis_index != -1)
                    throw std::runtime_error("multiple ellipsis in pattern");
                ellipsis_index = i;
            }

            Value cdr = p->getCdr();
            if (!isPair(cdr)) {
                pattern_len = i + 1;
                if (cdr == Value::Nil)
                    is_proper = true;
                break;
            }
            p = static_cast<PairObject*>(cdr.asPointer());
        }

        ssize_t expr_len = -1;
        auto q = static_cast<PairObject*>(expr.asPointer());
        for (size_t i = 0;; ++i) {
            Value cdr = q->getCdr();
            if (!isPair(cdr)) {
                expr_len = i + 1;
                if (is_proper != (cdr == Value::Nil))
                    return false;
                break;
            }
            q = static_cast<PairObject*>(cdr.asPointer());
        }

        if (expr_len < pattern_len)
            return false;

        ssize_t e = (ellipsis_index != -1) ? (ellipsis_index - 1) : (expr_len + 1);
        ssize_t m = (ellipsis_index != -1) ? (expr_len - pattern_len + ellipsis_index) : -1;

        p = static_cast<PairObject*>(pattern.asPointer());
        q = static_cast<PairObject*>(expr.asPointer());
        for (ssize_t i = 0;; ++i) {
            if (!match(p->getCar(), q->getCar(), mapping))
                return false;

            if (e <= i && i < m) {
                Value q_cdr = q->getCdr();
                q = static_cast<PairObject*>(q_cdr.asPointer());
            }
            else {
                Value p_cdr = (i == m) ? static_cast<PairObject*>(p->getCdr().asPointer())->getCdr()
                                       : p->getCdr();
                Value q_cdr = q->getCdr();
                if (!isPair(p_cdr) && !isPair(q_cdr)) {
                    if (is_proper)
                        return true;
                    return match(p_cdr, q_cdr, mapping);
                }
                if (!isPair(p_cdr) || !isPair(q_cdr))
                    return false;
                p = static_cast<PairObject*>(p_cdr.asPointer());
                q = static_cast<PairObject*>(q_cdr.asPointer());
            }
        }
    }

    return pattern == expr;
}


} // namespace nscheme
