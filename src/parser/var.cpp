#include "parser.hpp"

bool parser::is_datatype()
{
    bool result = false;
    int op = this->cursor;
    while(match(tkn_type::datamod)||match(tkn_type::datamod)){
        result = true;
    }
    if(result == true)
    {
        if(match(tkn_type::colon))
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
    this->cursor = op;
    return result;
}

bool parser::is_var_decl()
{
    bool result = false;
    int op = this->cursor;
    if(is_datatype())
    {
        if(match(tkn_type::identifer)){
            result = true;
        }
    }
    this->cursor = op;
    return false;
}

bool parser::is_var_def()
{
    bool result = false;
    int op = this->cursor;

    this->cursor = op;
    return false;
}

dtype parser::get_datatype()
{
    dtype node;

    return node;
}

decl_stmt parser::get_decl_stmt()
{
    decl_stmt node;

    return node;
}

def_stmt parser::get_def_stmt()
{
    def_stmt node;
    return node;
}