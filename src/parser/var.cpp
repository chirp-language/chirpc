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

bool parser::is_datatype(bool reset)
{
    bool result = false;
    int op = this->cursor;
    while(match(tkn_type::datamod)||match(tkn_type::datatype)){
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
    if(reset){
        this->cursor = op;
    }
    return result;
}

bool parser::is_var_decl(bool reset)
{
    bool result = false; 
    int op = this->cursor;
    if(is_datatype(false))
    {
        if(match(tkn_type::identifer)){
            result = true;
        }
    }
    if(reset){
        this->cursor = op;
    }
    return result;
}

bool parser::is_var_def(bool reset)
{
    bool result = false;
    int op = this->cursor;
    // Doesn't care about cast (yet)
    if(match(tkn_type::identifer) && match(tkn_type::assign_op))
    {
        result = true;
    }
    if(reset){
        this->cursor = op;
    }
    return result;
}

bool parser::is_var_decldef()
{
    bool result = false;
    int op = this->cursor;
    if(is_var_decl(false))
    {
        this->cursor--;
        if(is_var_def(false))
        {
            result = true;
        }
    }
    this->cursor = op;
    return result;
}

dtype parser::get_datatype()
{
    dtype node;
    //  Mods before the typename
    while(match(tkn_type::datamod))
    {
        node.tmods.push_back(static_cast<char>(get_dtypemod(peekb().value)));
    }

    // Get the typename, although it could also be a token identifier, it's a class object
    // but we don't care about that yet
    expect(tkn_type::datatype);

    node.tname = get_dtypename(this->peekb().value);

    if(!this->ok)
    {
        return node;
    }

    // Mods after the typename
    while(match(tkn_type::datamod))
    {
        node.tmods.push_back(static_cast<char>(get_dtypemod(peekb().value)));
    }
    expect(tkn_type::colon);
    return node;
}

decl_stmt parser::get_decl_stmt()
{
    decl_stmt node;
    node.type = get_datatype();
    node.ident = get_identifier();
    return node;
}

def_stmt parser::get_def_stmt()
{
    def_stmt node;
    node.ident = get_identifier();
    expect(tkn_type::assign_op);
    if(!this->ok){return node;}
    node.value = get_expr();
    return node;
}

decldef_stmt parser::get_decldef_stmt()
{
    decldef_stmt node;
    node.decl = get_decl_stmt();
    if(!this->ok){return node;}
    this->cursor--; // Go back to the identifier
    node.def = get_def_stmt();
    return node;
}