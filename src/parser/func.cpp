#include "parser.hpp"

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
    node.ident = get_identifier();
    node.args = get_arguments();
    return node;
}
