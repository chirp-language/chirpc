// This whole file is dedicated for parsing expressions
#include "parser.hpp"

mathop parser::get_math_op()
{
    mathop node;
    node.left = get_num_lit();
    
    if(this->peek().value.size() < 1){
        this->ok = false;
        helper e;
        e.l = this->peek().loc;
        e.msg = "Invalid math operator";
        // Could be out of range and segfault when writing the message
        e.type = helper_type::location_err;
        this->helpers.push_back(e);
        return node;
    }
    else
    {
        node.optype = this->peek().value.at(0);
    }
    expect(tkn_type::math_op);
    node.right = get_num_lit();
    return node;
}

// Doesn't actually do any operator precedence
// Cuz let's make the C backend handle
mathexpr parser::get_math_expr()
{
    mathexpr node;
    node.expr_type = exprtype::emath;
    node.operands.push_back(get_math_op());
    while(peek().type == tkn_type::math_op && this->ok)
    {
        cursor--; // hacky af boi
        node.operands.push_back(get_math_op());
    }

    return node;
}

staticexpr parser::get_static_expr()
{
    staticexpr node;
    node.expr_type = exprtype::estatic;
    node.value = this->get_literal();
    return node;
}

std::shared_ptr<expr> parser::get_expr()
{
    std::shared_ptr<expr> result;

    if(this->peekf().type == tkn_type::math_op)
    {
        result = std::make_shared<mathexpr>();
        *static_cast<mathexpr*>(result.get()) = get_math_expr();
    }
    else
    {
        result = std::make_shared<staticexpr>();
        *static_cast<staticexpr*>(result.get()) = get_static_expr();
    }

    return result;
}