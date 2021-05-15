#include "parser.hpp"
#include <string_view>

dtypename parser::get_dtypename(std::string const& txt)
{
    // The else aren't really needed, cuz you're supposed to already return
    if (txt == "int")
    {
        return dtypename::_int;
    }
    else if (txt == "float")
    {
        return dtypename::_float;
    }
    else if (txt == "double")
    {
        return dtypename::_double;
    }
    else if (txt == "char")
    {
        return dtypename::_char;
    }
    else if (txt == "byte")
    {
        return dtypename::_byte;
    }
    else if (txt == "bool")
    {
        return dtypename::_bool;
    }
    else if (txt == "none")
    {
        return dtypename::_none;
    }

    #ifndef NDEBUG
    // If this manages to reach here, then uhhh it's not supposed to happen
    this->ok = false;
    diagnostic e;
    e.type = diagnostic_type::global_err;
    e.msg = "Couldn't get typename from '" + txt + "', location unknown";
    this->diagnostics.push_back(std::move(e));
    return dtypename::_none;
    #else
    __builtin_unreachable();
    #endif
}

dtypemod parser::get_dtypemod(std::string const& txt)
{
    using namespace std::string_literals;
    dtypemod mod;

    if (txt == "ptr")
    {
        return dtypemod::_ptr;
    }
    else if (txt == "signed")
    {
        return dtypemod::_signed;
    }
    else if (txt == "unsigned")
    {
        return dtypemod::_unsigned;
    }
    else if (txt == "const")
    {
        return dtypemod::_const;
    }
    else if (txt == "func")
    {
        return dtypemod::_func;
    }

    this->ok = false;
    diagnostic e;
    e.type = diagnostic_type::global_err;
    e.msg = "Couldn't get type modifier from '" + txt + "', location unknown";
    this->diagnostics.push_back(std::move(e));
    return mod;
}

bool parser::is_datatype()
{
    return probe(tkn_type::datamod) || probe(tkn_type::datatype);
}

bool parser::is_var_decl()
{
    return is_datatype();
}

bool parser::is_var_assign()
{
    // Doesn't care about cast (yet)
    return probe(tkn_type::identifer) && peekf().type == tkn_type::assign_op;
}

exprtype parser::get_datatype()
{
    exprtype type;
    bool has_candidate = false;
    //  Mods before the typename
    while (match(tkn_type::datamod))
    {
        type.exttp.push_back(static_cast<std::byte>(get_dtypemod(peekb().value)));

        if (static_cast<dtypemod>(get_dtypemod(peekb().value)) == dtypemod::_ptr)
            has_candidate = true;
    }

    // Could also be a token identifier, but we don't care about that yet
    if (!match(tkn_type::datatype))
    {
        if (has_candidate)
            type.basetp = dtypename::_none;
    }
    else
    {
        type.basetp = get_dtypename(this->peekb().value);
    }
    if (!this->ok)
    {
        return type;
    }
    // Mods after the typename
    while (match(tkn_type::datamod))
    {
        type.exttp.push_back(static_cast<std::byte>(get_dtypemod(peekb().value)));
    }
    // expect(tkn_type::colon);
    return type;
}

std::shared_ptr<var_decl> parser::get_var_decl()
{
    auto node = std::make_shared<var_decl>();
    node->loc = loc_peek();
    node->var_type = get_datatype();
    expect(tkn_type::colon);
    node->ident = get_identifier();
    if (match(tkn_type::assign_op))
        node->init = get_expr(false);
    node->loc.end = loc_peekb();
    return node;
}

std::shared_ptr<assign_stmt> parser::get_assign_stmt()
{
    auto node = std::make_shared<assign_stmt>();
    node->loc = loc_peek();
    node->ident = get_identifier();
    expect(tkn_type::assign_op);

    if (!this->ok)
        return node;

    node->value = get_expr(false);
    node->loc.end = loc_peekb();
    return node;
}
