// This whole file is dedicated for parsing expressions
// Bink's Sketchy Expression Parsing "Algorithm"®
// 1 - When parsing an expression, get the range of the expression,
// and turn all the components of the expressions into operands
// 2 - Go at the last operand of the expression, and go reverse, to find
// the operator with lowest precedence
// 3 - Split the range in two halfs, and do the same thing for these
// (also check if there's even an operator, otherwise just end there)
// This is probably already an existing algorithm, I just improvised something lol
#include "parser.hpp"

#include <iostream>

bool parser::is_operand(bool reset)
{
    bool result = false;
    int op = this->cursor;
    if(
        is_func_call(false)||
        is_identifier(false)||
        match(tkn_type::literal)
        )
    {
        result = true;
    }
    if(reset)
    {
        this->cursor = op;
    }
    return result;
}

subexpr parser::get_subexpr(std::vector<operand> range)
{
    subexpr node;
    return node;
}

operand parser::get_operand()
{
    operand result;
    if(is_func_call(true))
    {
        result.type = optype::call;
        result.node = std::make_unique<func_call_stmt>(get_fcall());
    }
    else if(is_identifier(true))
    {
        result.type = optype::ident;
        result.node = std::make_unique<identifier>(get_identifier());
    }
    else if(peek().type == tkn_type::literal)
    {
        result.type = optype::lit;
        result.node = get_literal();
    }
    else
    {
        std::string v = peek().value;
        result.type = optype::op;
        exprop op;
        if(v == "("){
            op.type = '(';
            expect(tkn_type::lparen);
        }
        else if(v == ")"){
            op.type = ')';
            expect(tkn_type::rparen);
        }
        else if(v == "*"){
            op.type = '*';
            expect(tkn_type::math_op);
        }
        else if(v == "/"){
            op.type = '/';
            expect(tkn_type::math_op);
        }
        else if(v == "+"){
            op.type = '+';
            expect(tkn_type::math_op);
        }
        else if(v == "-"){
            op.type = '-';
            expect(tkn_type::math_op);
        }
        else if(v == "deref"){
            expect(tkn_type::deref_op);
        }
        else if(v == "ref"){
            expect(tkn_type::ref_op);
        }
        else if(v == "as"){
            expect(tkn_type::as_op);
        }
        else{
            result.type = optype::invalid;
            helper e;
            e.l = peek().loc;
            e.msg = "Invalid operand";
            std::cout<<"Not an operand"<<std::endl;
            e.type = helper_type::location_err;
            this->ok = false;
            this->helpers.push_back(e);
            return result;
        }
        result.node = std::make_unique<exprop>(op);
    }
    return result;
}

expr parser::get_expr()
{
    expr node;
    if(!is_operand(true))
    {
        helper e;
        e.l = peek().loc;
        e.msg = "Invalid expression";
        e.type = helper_type::location_err;
        this->helpers.push_back(e);
        this->ok = false;
        return node;
    }

    // Get's the expression's range
    std::vector<operand> range;
    int op = this->cursor;
    this->cursor = op;
    int s = this->cursor;
    int e = -1;

    while(
        (is_operand(false))
        &&
        (match(tkn_type::math_op)||match(tkn_type::deref_op)||match(tkn_type::ref_op)||match(tkn_type::as_op)||match(tkn_type::lparen)||match(tkn_type::rparen))
    ){
        int v = 1;
    }
    e = this->cursor;
    this->cursor = s;

    while(this->cursor < e)
    {
        operand tmp = get_operand();
        if(tmp.type == optype::invalid){
            // Does an error twice now I guess
            helper e;
            e.l = peek().loc;
            e.msg = "Expression parsing failed";
            e.type = helper_type::line_err;
            this->helpers.push_back(e);
            this->ok = false;
            return node;
        }
        else
        {
            range.push_back(tmp);
        }
    }

    std::cout<<"==--- Expression Range Dump ---=="<<std::endl;
    for(operand o : range){
        std::cout<<o.dump(0)<<std::endl;
    }

    int c = 123;
    return node;
}