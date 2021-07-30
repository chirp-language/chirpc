#include "parser.hpp"

parameters parser::parse_parameters()
{
    parameters node;
    node.loc.begin = loc_peek();
    expect(tkn_type::lparen);

    if (!match(tkn_type::rparen))
    {
        do
        {
            node.body.push_back(static_cast<std::unique_ptr<var_decl>>(parse_parameter()));
        } while (match(tkn_type::comma));

        expect(tkn_type::rparen);
    }
    node.loc.end = loc_peekb();
    return node;
}

nodeh<func_decl> parser::parse_func_decl()
{
    // Inherited stuff
    auto loc = loc_peekb();

    if (!this->ok)
        return nullptr;
    auto data_type = parse_datatype();
    if (!this->ok)
        return nullptr;
    auto ident = parse_identifier();
    if (!this->ok)
        return nullptr;
    auto params = parse_parameters();
    if (!this->ok)
        return nullptr;
    nodeh<func_decl> node;
    if (match(tkn_type::lbrace))
    {
        node = new_node<func_def>();
        static_cast<func_def&>(*node).body = parse_compound_stmt();
    }
    else
    {
        expect(tkn_type::semi);
        node = new_node<func_decl>();
    }
    node->loc = loc;
    node->result_type = std::move(data_type);
    node->ident = std::move(ident);
    node->params = std::move(params);
    node->loc.end = loc_peekb();

    return node;
}

arguments parser::parse_arguments()
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
                this->ok = false;
                diagnostic(diagnostic_type::location_err)
                    .at(loc_peekb())
                    .reason("Missing an argument before comma")
                    .report(this->diagnostics);
                while (match(tkn_type::comma))
                    ;
            }
            node.body.push_back(parse_expr(false));
            if (match(tkn_type::comma))
                continue;
            else if (match(tkn_type::rparen))
                break;
            else
            {
                this->ok = false;
                diagnostic(diagnostic_type::location_err)
                    .at(loc_peekb())
                    .reason("Expected ')' or ','")
                    .report(this->diagnostics);
                break;
            }
        }
    }

    node.loc.end = loc_peekb();

    return node;
}

nodeh<func_call> parser::parse_fcall(exprh callee)
{
    auto lbeg = callee->loc.begin;
    auto node = new_node<func_call>(std::move(callee), parse_arguments());
    node->loc = location_range(lbeg, loc_peekb());
    return node;
}
