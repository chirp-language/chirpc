#include "parser.hpp"

identifier parser::parse_identifier()
{
    identifier node;
    node.loc = loc_peek();
    node.name = peek().value;
    expect(tkn_type::identifer);
    return node;
}

qual_identifier parser::parse_qual_identifier() {
    qual_identifier node;
    node.loc = loc_peek();
    while (match(tkn_type::identifer))
    {
        token const& ns = peekb();
        node.parts.push_back(identifier::from(std::string(ns.value), loc_peekb()));
        if (!match(tkn_type::period))
            break;
    }
    node.loc.end = loc_peekb();
    return node;
}
