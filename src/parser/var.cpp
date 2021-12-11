#include "parser.hpp"
#include <string_view>
#include <algorithm>

static dtypename get_dtypename(tkn_type tok)
{
    switch (tok) {
        case tkn_type::dt_int:
            return dtypename::_int;
        case tkn_type::dt_long:
            return dtypename::_long;
        case tkn_type::dt_float:
            return dtypename::_float;
        case tkn_type::dt_double:
            return dtypename::_double;
        case tkn_type::dt_char:
            return dtypename::_char;
        case tkn_type::dt_byte:
            return dtypename::_byte;
        case tkn_type::dt_bool:
            return dtypename::_bool;
        case tkn_type::dt_none:
            return dtypename::_none;
        default: ;
    }
    // Unknown type token
    chirp_unreachable("get_dtypename");
}

static dtypemod get_dtypemod(tkn_type tok)
{
    switch (tok) {
        case tkn_type::dm_ptr:
            return dtypemod::_ptr;
        case tkn_type::dm_signed:
            return dtypemod::_signed;
        case tkn_type::dm_unsigned:
            return dtypemod::_unsigned;
        case tkn_type::dm_const:
            return dtypemod::_const;
        case tkn_type::kw_func:
            return dtypemod::_func;
        default: ;
    }
    // Unknown type token
    chirp_unreachable("get_dtypemod");
}

bool parser::is_datatype()
{
    return probe_range(tkn_type::datatype_S, tkn_type::datatype_E);
}

bool parser::is_datamod()
{
    return probe_range(tkn_type::datamod_S, tkn_type::datamod_E);
}

bool parser::is_type()
{
    return is_datatype() or is_datamod();
}

bool parser::is_var_decl()
{
    return is_type();
}

basic_type parser::parse_datatype()
{
    basic_type type;
    bool has_candidate = false;
    // Mods before the typename
    while (is_datamod())
    {
        type.exttp.push_back(static_cast<std::byte>(get_dtypemod(peek().type)));

        if (static_cast<dtypemod>(get_dtypemod(peek().type)) == dtypemod::_ptr)
            has_candidate = true;
        skip();
    }
    // Keep the modifiers in reverse order for easier manipulation
    std::reverse(type.exttp.begin(), type.exttp.end());

    // Could also be a token identifier, but we don't care about that yet
    if (is_datatype())
    {
        type.basetp = get_dtypename(peek().type);
        skip();
    }
    else if (has_candidate)
    {
        type.basetp = dtypename::_none;
    }
    else
    {
        type.basetp = dtypename::_none;
        this->ok = false;
        diagnostic(diagnostic_type::location_err)
            .at(loc_peek())
            .reason("Expected a data type")
            .report(diagnostics);
    }

    return type;
}

nodeh<var_decl> parser::parse_var_decl()
{
    auto node = new_node<var_decl>();
    node->loc = loc_peek();
    node->type = parse_datatype();
    expect(tkn_type::colon);
    node->ident = parse_identifier();
    if (match(tkn_type::assign_op))
        node->init = parse_expr(false);
    expect(tkn_type::semi);
    node->loc.end = loc_peekb();
    return node;
}

nodeh<var_decl> parser::parse_parameter()
{
    auto node = new_node<var_decl>();
    node->loc = loc_peek();
    node->type = parse_datatype();
    if (match(tkn_type::colon))
    {
        node->ident = parse_identifier();
    }
    node->loc.end = loc_peekb();
    return node;
}

nodeh<assign_stmt> parser::parse_assign_stmt(exprh target)
{
    auto node = new_node<assign_stmt>();
    node->loc = target->loc;
    node->target = std::move(target);
    node->assign_op = peekb().type;
    node->assign_loc = loc_peekb();
    if (!this->ok)
        return node;

    node->value = parse_expr(false);
    expect(tkn_type::semi);
    node->loc.end = loc_peekb();
    return node;
}
