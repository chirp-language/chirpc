#include "parser.hpp"

bool parser::is_identifier(bool reset)
{
    bool result = false;
    int op = this->cursor;

    do
    {
        if (match(tkn_type::identifer))
        {
            result = true;
        }
        else
        {
            result = false;
        }
    } while (match(tkn_type::comma));

    if (reset)
    {
        this->cursor = op;
    }

    return result;
}

bool parser::is_lop(bool reset)
{
    bool result = false;
    int op = this->cursor;

    if(
        is_identifier(false)        ||
        match(tkn_type::ref_op)     ||
        match(tkn_type::deref_op)   ||
        match(tkn_type::as_op)
    ){
        result = true;
    }

    if(reset)
    {
        this->cursor = op;
    }
    return result;
}

bool parser::is_lvalue(bool reset)
{
    bool result = false;
    int op = this->cursor;

    while(is_lop(false)){
        result = true;
    }

    if(reset)
    {
        this->cursor = op;
    }

    return result;
}

identifier parser::get_identifier()
{
    identifier node;
    while (match(tkn_type::identifer))
    {
        token ns = peekb();
        if (match(tkn_type::period))
        {
            node.namespaces.push_back(ns.value);
        }
        else
        {
            node.name = ns.value;
            break;
        }
    }
    return node;
}

lvalue parser::get_lvalue()
{
    lvalue node;

    return node;
}