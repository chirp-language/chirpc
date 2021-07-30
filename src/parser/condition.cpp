// Parsing of conditional statements and stuff like that

#include "parser.hpp"

nodeh<conditional_stmt> parser::parse_cond()
{
    auto node = new_node<conditional_stmt>();
    node->loc = loc_peekb();
    node->cond = parse_expr(true);
    expect(tkn_type::lbrace);
    node->true_branch = parse_compound_stmt();
    if (match(tkn_type::kw_elif)) // Shorthand for else if
        node->false_branch = parse_cond();
    else if (match(tkn_type::kw_else))
    {
        expect(tkn_type::lbrace);
        node->false_branch = parse_compound_stmt();
    }
    node->loc.end = loc_peekb();
    return node;
}

nodeh<iteration_stmt> parser::parse_iter()
{
	auto node = new_node<iteration_stmt>();
	node->loc = loc_peekb();
	node->cond = parse_expr(true);
	expect(tkn_type::lbrace);
	node->loop_body = parse_compound_stmt();
	node->loc.end = loc_peekb();
	return node;
}
