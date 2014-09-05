#include "node.hpp"

namespace nscheme {

std::string LambdaNode::toString() const {
    std::string buffer("(lambda ");
    if (is_variable_) {
        if (arg_names_.size() == 1) {
            buffer += arg_names_[0].toString();
        } else {
            buffer.push_back('(');
            for (size_t i = 0; i < arg_names_.size(); ++i) {
                buffer += arg_names_[i].toString();
                if (i + 2 < arg_names_.size())
                    buffer.push_back(' ');
                else if (i + 2 == arg_names_.size())
                    buffer += " . ";
            }
            buffer.push_back(')');
        }
    } else {
        buffer.push_back('(');
        for (size_t i = 0; i < arg_names_.size(); ++i) {
            if (i != 0)
                buffer.push_back(' ');
            buffer += arg_names_[i].toString();
        }
        buffer.push_back(')');
    }
    for (DefineNode* def : defines_) {
        buffer.push_back(' ');
        buffer += def->toString();
    }
    for (ExprNode* expr : exprs_) {
        buffer.push_back(' ');
        buffer += expr->toString();
    }
    buffer.push_back(')');
    return buffer;
}

}
