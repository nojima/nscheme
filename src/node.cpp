#include "node.hpp"
#include "code.hpp"


namespace {

using namespace nscheme;


bool isSelfEvaluating(Value value) {
    if (value.isInteger()) return true;
    if (value.isCharacter()) return true;
    if (value == Value::True) return true;
    if (value == Value::False) return true;
    if (value.isPointer()) {
        auto p1 = dynamic_cast<StringObject*>(value.asPointer());
        if (p1 != nullptr)
            return true;
        auto p2 = dynamic_cast<RealObject*>(value.asPointer());
        if (p2 != nullptr)
            return true;
    }
    return false;
}

}   // namespace


namespace nscheme {


void VariableNode::codegen(Code& code) {
    code.main.push_back(new LoadVariableInst(name_));
}


std::string VariableNode::toString() const {
    return name_.toString();
}


void LiteralNode::codegen(Code& code) {
    code.main.push_back(new LoadLiteralInst(value_));
}


std::string LiteralNode::toString() const {
    if (isSelfEvaluating(value_))
        return value_.toString();
    else
        return "'" + value_.toString();
}


void ProcedureCallNode::codegen(Code& code) {
    for (ExprNode* node: operand_)
        node->codegen(code);
    callee_->codegen(code);
    code.main.push_back(new ApplyInst(operand_.size()));
}


std::string ProcedureCallNode::toString() const {
    std::string buffer("{");
    buffer += callee_->toString();
    for (Node* p : operand_) {
        buffer.push_back(' ');
        buffer += p->toString();
    }
    buffer.push_back('}');
    return buffer;
}


void DefineNode::codegen(Code& code) {
    expr_->codegen(code);
    code.main.push_back(new DefineInst(name_));
}


std::string DefineNode::toString() const {
    std::string buffer("[define ");
    buffer += name_.toString();
    buffer.push_back(' ');
    buffer += expr_->toString();
    buffer.push_back(']');
    return buffer;
}


void LambdaNode::codegen(Code& code) {
    Code subcode;
    for (size_t i = 0; i < nodes_.size(); ++i) {
        nodes_[i]->codegen(subcode);
        if (i != nodes_.size() - 1)
            subcode.main.push_back(new DiscardInst());
    }
    subcode.main.push_back(new ReturnInst());

    LabelInst* label = new LabelInst;
    code.sub.push_back(label);
    code.sub.insert(code.sub.end(), subcode.main.begin(), subcode.main.end());
    code.sub.insert(code.sub.end(), subcode.sub.begin(), subcode.sub.end());

    code.main.push_back(new LoadClosureInst(label, arg_names_));
}


std::string LambdaNode::toString() const {
    std::string buffer("<lambda ");
    if (variable_args_) {
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
    /*
    if (!local_names_.empty()) {
        buffer += " [local-names";
        for (Symbol symbol : local_names_) {
            buffer.push_back(' ');
            buffer += symbol.toString();
        }
        buffer.push_back(']');
    }
    */
    for (Node* node : nodes_) {
        buffer.push_back(' ');
        buffer += node->toString();
    }
    buffer.push_back('>');
    return buffer;
}


void IfNode::codegen(Code& code) {
    Code then_code;
    then_node_->codegen(then_code);
    then_code.main.push_back(new BranchReturnInst());

    LabelInst* then_label = new LabelInst;
    code.sub.push_back(then_label);
    code.sub.insert(code.sub.end(), then_code.main.begin(), then_code.main.end());
    code.sub.insert(code.sub.end(), then_code.sub.begin(), then_code.sub.end());

    Code else_code;
    else_node_->codegen(else_code);
    else_code.main.push_back(new BranchReturnInst());

    LabelInst* else_label = new LabelInst;
    code.sub.push_back(else_label);
    code.sub.insert(code.sub.end(), else_code.main.begin(), else_code.main.end());
    code.sub.insert(code.sub.end(), else_code.sub.begin(), else_code.sub.end());

    cond_node_->codegen(code);
    code.main.push_back(new BranchInst(then_label, else_label));
}


std::string IfNode::toString() const {
    std::string buffer("<if ");
    buffer += cond_node_->toString();
    buffer.push_back(' ');
    buffer += then_node_->toString();
    buffer.push_back(' ');
    buffer += else_node_->toString();
    buffer.push_back('>');
    return buffer;
}


void AssignmentNode::codegen(Code& code) {
    expr_->codegen(code);
    code.main.push_back(new AssignInst(name_));
}


std::string AssignmentNode::toString() const {
    std::string buffer("<set! ");
    buffer += name_.toString();
    buffer.push_back(' ');
    buffer += expr_->toString();
    buffer.push_back('>');
    return buffer;
}


}
