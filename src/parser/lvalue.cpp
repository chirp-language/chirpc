#include "parser.hpp"

bool parser::is_identifier()
{
    return probe(tkn_type::identifer);
}

bool parser::is_lop()
{
    return (
        is_identifier()             ||
        probe(tkn_type::ref_op)     ||
        probe(tkn_type::deref_op)   ||
        probe(tkn_type::as_op)
    );
}

bool parser::is_lvalue()
{
    return is_lop();
}

identifier parser::get_identifier()
{
    identifier node;
    node.loc = loc_peek();
    while (match(tkn_type::identifer))
    {
        token const& ns = peekb();
        if (match(tkn_type::period))
        {
            node.namespaces.push_back(ns.value);
        }
        else
        {
            node.name = ns.value;
            break;
        }
    }
    node.loc.end = loc_peekb();
    return node;
}
