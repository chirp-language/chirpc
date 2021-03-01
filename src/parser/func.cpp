#include "parser.hpp"

bool parser::is_params(bool reset)
{
    bool result = false;
    int op = this->cursor;

    if(match(tkn_type::lparen))
    {
        while(is_var_decl(false)){}
        if(match(tkn_type::rparen))
        {
            result = true;
        }
    }

    if(reset)
    {
        this->cursor = op;
    }
    return result;
}

bool parser::is_func_decl(bool reset)
{
    bool result = false;
    int op = this->cursor;

    if(match(tkn_type::kw_func))
    {
        if(is_datatype(false) && match(tkn_type::identifer))
        {
            if(is_params(false))
            {
                result = true;
            }
        }
    }

    if(reset){
        this->cursor = op;
    }

    return result;
}

bool parser::is_func_def(bool reset)
{
    bool result = false;
    int op = this->cursor;

    if(is_func_decl(false))
    {
        if(match(tkn_type::lbrace))
        {
            result = true;
        }
    }

    if(reset){
        this->cursor = op;
    }

    return result;
}

bool parser::is_func_call()
{
    bool result = false;
    int og = this->cursor;

    if (match(tkn_type::identifer))
    {
        if (match(tkn_type::lparen))
        {
            result = true;
        }
        else if (match(tkn_type::period))
        {
            while (match(tkn_type::identifer) && match(tkn_type::period));

            if (match(tkn_type::lparen))
            {
                result = true;
            }
        }
    }

    this->cursor = og;
    return result;
}

arguments parser::get_arguments()
{
    arguments node;
    expect(tkn_type::lparen);
    
    while (!match(tkn_type::rparen) && !match(tkn_type::eof) && this->ok)
    {
        match(tkn_type::comma);
        node.body.push_back(get_expr());
    }
    
    return node;
}

func_call_stmt parser::get_fcall()
{
    func_call_stmt node;
    node.type = stmt_type::fcall;
    node.ident = get_identifier();
    node.args = get_arguments();
    return node;
}
