#include "parser.hpp"

bool parser::is_params()
{
    bool result = false;
    int op = this->cursor;

    if (match(tkn_type::lparen))
    {
        while (is_var_decl() && match(tkn_type::comma));
        
        if (probe(tkn_type::rparen))
        {
            result = true;
        }
    }
    cursor = op;
    return result;
}

parameters parser::get_parameters()
{
    parameters node;
    expect(tkn_type::lparen);

    do
    {
        node.body.push_back(get_var_decl());
    } while (match(tkn_type::comma));

    expect(tkn_type::rparen);
    return node;
}

std::shared_ptr<func_decl> parser::get_func_decl()
{
    // Inherited stuff
    auto loc = loc_peekb();

    if (!this->ok)
        return nullptr;
    auto data_type = get_datatype();
    if (!this->ok)
        return nullptr;
    auto ident = get_identifier();
    if (!this->ok)
        return nullptr;
    auto params = get_parameters();
    if (!this->ok)
        return nullptr;
    std::shared_ptr<func_decl> node;
    if (match(tkn_type::lbrace))
    {
        node = std::make_shared<func_def>();
        static_cast<func_def&>(*node).body = get_compound_stmt();
    }
    else
    {
        node = std::make_shared<func_decl>();
    }
    node->loc = loc;
    node->data_type = std::move(data_type);
    node->ident = std::move(ident);
    node->params = std::move(params);
    node->loc.end = loc_peekb();

    return node;
}

arguments parser::get_arguments()
{
    arguments node;
    node.loc = loc_peekb();

    // Get's the range for each expression
    // This doesn't work very well, could be improved
    if (!match(tkn_type::rparen))
    {
        while (this->ok)
        {
            if (match(tkn_type::comma))
            {
                // QoL thing
                diagnostic e;
                e.l = loc_peekb();
                e.msg = "Missing an argument before comma";
                e.type = diagnostic_type::location_err;
                this->diagnostics.show(e);
                this->ok = false;
                while (match(tkn_type::comma))
                    ;
            }
            node.body.push_back(get_expr(false));
            if (match(tkn_type::comma))
                continue;
            else if (match(tkn_type::rparen))
                break;
            else
            {
                diagnostic e;
                e.l = loc_peekb();
                e.msg = "Expected ')' or ','";
                e.type = diagnostic_type::location_err;
                this->diagnostics.show(e);
                this->ok = false;
                break;
            }
        }
    }

    node.loc.end = loc_peekb();

    return node;
}

std::shared_ptr<func_call> parser::get_fcall(exprh callee)
{
    auto lbeg = callee->loc.begin;
    auto node = std::make_shared<func_call>(std::move(callee), get_arguments());
    node->loc = location_range(lbeg, loc_peekb());
    return node;
}
