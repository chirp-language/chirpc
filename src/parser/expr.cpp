// This whole file is dedicated for parsing expressions
#include "parser.hpp"

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

subexpr parser::get_subexpr(int start, int end)
{
    subexpr node;
    // ()   0
    // as deref ref     1
    // + -  2 
    // * /  3
    int i = start;
    int op = 0;
    int x;
    while(i < end)
    {
        std::string v = tkns.at(i).value;
        
        if(v == "(" || v == ")")
        {
            if(0 >= op){
                op = 0;
                x = i;
            }
        }
        else if(v == "as" || v == "deref" || v=="ref")
        {
            if(1 > op){
                op = 1;
                x = i;
            }
        }
        else if(v == "+" || v == "-")
        {
            if(2 > op){
                op = 2;
                x = i;
            }
        }
        else if(v == "*" || v == "/")
        {
            if(3 > op){
                op = 3;
                x = i;
            }
        }
        i++;
    }
    

    return node;
}

operand parser::get_operand()
{
    operand result;
    
    // Original Position
    int op = this->cursor;
    if(is_operand(false))
    {
        if(
            match(tkn_type::math_op)||
            match(tkn_type::deref_op)||
            match(tkn_type::ref_op)||
            match(tkn_type::as_op)
        ){
            result.type = optype::subexpr;
            // Gets the subexpr range
            this->cursor = op;
            int s = this->cursor;
            int e = -1;
            while(
                (is_operand(false))
                &&
                (match(tkn_type::math_op)||match(tkn_type::deref_op)||match(tkn_type::ref_op)||match(tkn_type::as_op))
            ){}
            e = this->cursor;
            result.node = std::make_shared<subexpr>(get_subexpr(s,e));
        }
        else
        {
            this->cursor = op;
            if(is_func_call(true))
            {
                result.node = std::make_unique<func_call_stmt>(get_fcall());
            }
            else if(is_identifier(true))
            {
                result.node = std::make_unique<identifier>(get_identifier());
            }
            else if(peek().type == tkn_type::literal)
            {
                result.node = get_literal();
            }
            else
            {
                helper e;
                e.l = peek().loc;
                e.msg = "Unexpected token in operand parsing";
                e.type = helper_type::location_err;
                this->helpers.push_back(e);
                this->ok = false;
            }
        }
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
    node.root = get_operand();
    return node;
}