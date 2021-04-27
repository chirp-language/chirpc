#include "parser.hpp"
#include <string_view>

dtypename parser::get_dtypename(std::string txt)
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
    this->diagnostics.push_back(e);
    #else
    __builtin_unreachable();
    #endif
}

dtypemod parser::get_dtypemod(std::string txt)
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
    this->diagnostics.push_back(e);
    return mod;
}

bool parser::is_datatype()
{
    return probe(tkn_type::datamod) || probe(tkn_type::datatype);
}

bool parser::is_var_decl()
{
    bool result = false;
    if (is_datatype() && peekf().type == tkn_type::colon)
    {
        cursor += 2;
        if (probe(tkn_type::identifer))
            result = true;
        cursor -= 2;
    }
    return result;
}

bool parser::is_var_def()
{
    // Doesn't care about cast (yet)
    return probe(tkn_type::identifer) && peekf().type == tkn_type::assign_op;
}

bool parser::is_var_decldef()
{
    // Continuation of a decl
    return probe(tkn_type::assign_op);
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

std::shared_ptr<decl_stmt> parser::get_decl_stmt()
{
    auto node = std::make_shared<decl_stmt>();
    node->loc = loc_peek();
    node->type = stmt_type::decl;
    node->data_type = get_datatype();
    expect(tkn_type::colon);
    node->ident = get_identifier();
    node->loc.end = loc_peekb();
    return node;
}

std::shared_ptr<def_stmt> parser::get_def_stmt()
{
    auto node = std::make_shared<def_stmt>();
    node->loc = loc_peek();
    node->type = stmt_type::def;
    node->ident = get_identifier();
    expect(tkn_type::assign_op);

    if (!this->ok)
        return node;

    node->value = get_expr(false);
    node->loc.end = loc_peekb();
    return node;
}

std::shared_ptr<decl_def_stmt> parser::get_decldef_stmt(std::shared_ptr<decl_stmt> decl)
{
    auto node = std::make_shared<decl_def_stmt>();
    node->type = stmt_type::decldef;
    node->decl = std::move(decl);

    this->cursor--; // Go back to the identifier
    node->def = get_def_stmt();
    if (this->ok)
        node->loc = location_range(node->decl->loc.begin, node->def->loc.end);
    return node;
}
