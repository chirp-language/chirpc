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

bool parser::is_func_decl()
{
    bool result = false;
    int op = this->cursor;

    if (match(tkn_type::kw_func))
    {
        if (is_datatype() && is_identifier())
        {
            if (is_params())
            {
                return true;
            }
        }
    }

    return false;
}

bool parser::is_func_def()
{
    bool result = false;
    int op = this->cursor;

    if (is_func_decl())
    {
        if (probe(tkn_type::lbrace))
        {
            return true;
        }
    }

    return false;
}

bool parser::is_func_call()
{
    // TODO: Needs rework | form: <expr> ( <arg_list>? )
    bool result = false;
    int og = this->cursor;

    if (is_identifier())
    {
        if (match(tkn_type::lparen))
        {
            result = true;

            // Just skips until lparenA
            int depth = 1;
            while (depth != 0)
            {
                if (peek().type == tkn_type::lparen)
                {
                    depth++;
                }
                if (peek().type == tkn_type::rparen)
                {
                    depth--;
                }
                this->cursor++;
            }
        }
        else
        {
            result = false;
        }
    }
    this->cursor = og;

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
    auto node = std::make_shared<func_decl>();

    // Inherited stuff
    node->loc = loc_peek();

    expect(tkn_type::kw_func);
    if (!this->ok)
    {
        return node;
    }
    node->data_type = get_datatype();
    if (!this->ok)
    {
        return node;
    }
    node->ident = get_identifier();
    if (!this->ok)
    {
        return node;
    }
    node->params = get_parameters();
    node->loc.end = loc_peekb();

    return node;
}

std::shared_ptr<func_def> parser::get_func_def()
{
    auto node = std::make_shared<func_def>();

    // Inherited stuff
    node->loc = loc_peek();

    expect(tkn_type::kw_func);
    if (!this->ok)
    {
        return node;
    }
    node->data_type = get_datatype();
    if (!this->ok)
    {
        return node;
    }
    node->ident = get_identifier();
    if (!this->ok)
    {
        return node;
    }
    node->params = get_parameters();
    if (!this->ok)
    {
        return node;
    }
    node->body = get_compound_stmt();
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
                this->diagnostics.push_back(std::move(e));
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
                this->diagnostics.push_back(std::move(e));
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
