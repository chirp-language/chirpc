#include "parser.hpp"

dtypename parser::get_dtypename(std::string txt)
{
    // The else aren't really needed, cuz you're supposed to already return
    if(txt == "int"){return dtypename::_int;}
    else if(txt == "float"){return dtypename::_float;}
    else if(txt == "double"){return dtypename::_double;}
    else if(txt == "char"){return dtypename::_char;}
    else if(txt == "byte"){return dtypename::_byte;}
    else if(txt == "bool"){return dtypename::_bool;}
    else if(txt == "none"){return dtypename::_none;}
    // If this manages to reach here, then uhhh it's not supposed to happen
    this->ok = false;
    helper e;
    e.type = helper_type::global_err;
    e.msg = "Couldn't get typename from '" + txt + "', location unknown";
    this->helpers.push_back(e);
}

dtypemod parser::get_dtypemod(std::string txt)
{
    dtypemod mod;
    if(txt == "ptr"){return dtypemod::_ptr;}
    else if(txt == "signed"){return dtypemod::_signed;}
    else if(txt == "unsigned"){return dtypemod::_unsigned;}
    else if(txt == "const"){return dtypemod::_const;}
    else if(txt == "func"){return dtypemod::_func;}
    this ->ok = false;
    helper e;
    e.type = helper_type::global_err;
    e.msg = "Couldn't get type modifier from '" + txt + "', location unknown";
    this->helpers.push_back(e);
    return mod;
}

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