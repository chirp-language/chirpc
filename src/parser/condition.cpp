// Parsing of conditional statements and stuff like that

#include "parser.hpp"

std::shared_ptr<conditional_stmt> parser::get_cond()
{
    auto node = std::make_shared<conditional_stmt>();
    node->loc = loc_peekb();
    node->cond = get_expr(true);
    expect(tkn_type::lbrace);
    node->true_branch = get_compound_stmt();
    if (match(tkn_type::kw_elif)) // Shorthand for else if
        node->false_branch = get_cond();
    else if (match(tkn_type::kw_else))
    {
        expect(tkn_type::lbrace);
        node->false_branch = get_compound_stmt();
    }
    node->loc.end = loc_peekb();
    return node;
}
