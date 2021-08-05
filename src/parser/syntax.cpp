#include "parser.hpp"

#include <cstdlib>
#include <string.h>
#include <new>
#include <iostream>
#include <string>

// Returns true if character value overflows
static inline bool parse_character_value(std::string const& value, integer_value& out)
{
    bool ovf = false;
    out.val = 0;
    for (auto ch : value)
    {
        if ( out.val & (0xFFUL << 56) )
            ovf = true;
        out.val <<= 8;
        out.val |= static_cast<uint8_t>(ch);
    }
    return ovf;
}

// Returns true on error
static inline bool parse_integer(std::string const& value, integer_value& out)
{
    char const* end = value.data() + value.size();
    char const* oend = end;
    out.val = std::strtoll(value.data(), const_cast<char**>(&end), 0);
    return end != oend;
}

string_literal parser::build_string_lit(token_location loc, std::string&& value)
{
    string_literal node;
    node.loc = loc;
    node.value = std::move(value);
    return node;
}

integral_literal parser::build_integral_lit(token_location loc, integer_value value, dtypename type)
{
    integral_literal node;
    node.type.basetp = type;
    node.cat = exprcat::rval;
    node.loc = loc;
    node.value = value;
    return node;
}

integral_literal parser::build_bool_lit(token_location loc, bool value)
{
    return build_integral_lit(loc, integer_value(static_cast<int64_t>(value)), dtypename::_bool);
}

nullptr_literal parser::build_null_ptr_lit(token_location loc)
{
    nullptr_literal node;
    node.type.basetp = dtypename::_none;
    node.type.exttp.push_back(static_cast<std::byte>(dtypemod::_ptr));
    node.cat = exprcat::rval;
    node.loc = loc;
    return node;
}

exprh parser::parse_str_or_char_lit(token_location loc, std::string const& tok_value)
{
    bool is_character = tok_value.at(0) == '\'';
    std::string value(tok_value);
    value.pop_back();
    value.erase(0, 1);
    if (is_character)
    {
        dtypename typ = dtypename::_char;
        if (value.size() != 1)
        {
            typ = dtypename::_long;
            diagnostic(diagnostic_type::location_warning)
                .at(loc)
                .reason("Character value is not single-character")
                .report(this->diagnostics);
        }
        integer_value v;
        if (parse_character_value(value, v))
        {
            diagnostic(diagnostic_type::location_warning)
                .at(loc)
                .reason("Character constant value overflows")
                .report(this->diagnostics);
        }
        return new_node<integral_literal>(build_integral_lit(loc, v, typ));
    }
    return new_node<string_literal>(build_string_lit(loc, std::move(value)));
}

exprh parser::parse_literal()
{
    auto const& val = peek().value;
    token_location loc = loc_peek();
    skip();

    if (val.at(0) == '"' || val.at(0) == '\'')
        return parse_str_or_char_lit(loc, val);
    else
    {
        integer_value v;
        if (parse_integer(val, v))
        {
            diagnostic(diagnostic_type::location_err)
                .at(loc)
                .reason("Invalid integer literal")
                .report(this->diagnostics);
        }
        int64_t ext_v = v.val & 0xFFFF'FFFF'0000'0000;
        // This is bad, too bad
        dtypename typ = (ext_v == 0 || ext_v == 0xFFFF'FFFF'0000'0000) ? dtypename::_int : dtypename::_long;
        return new_node<integral_literal>(build_integral_lit(loc, v, typ));
    }
    return nullptr;
}

nodeh<entry_decl> parser::parse_entry()
{
    auto node = new_node<entry_decl>();
    node->loc = loc_peekb();
    expect(tkn_type::lbrace);
    node->code = parse_compound_stmt();
    node->loc.end = loc_peekb();
    return node;
}

nodeh<import_decl> parser::parse_import()
{
    auto node = new_node<import_decl>();
    node->loc = loc_peekb();
    expect(tkn_type::literal);
    node->filename = peekb().value;
    node->filename.erase(0, 1);
    node->filename.pop_back();
    node->loc.end = loc_peekb();
    return node;
}

nodeh<namespace_decl> parser::parse_namespace()
{
    auto node = new_node<namespace_decl>();
    node->loc = loc_peekb();
    node->ident = parse_identifier();

    expect(tkn_type::lbrace);

    while(this->ok && !match(tkn_type::rbrace) && !match(tkn_type::eof))
    {
        switch (peek().type)
        {
            case tkn_type::kw_entry:
            {
                this->ok = false;
                diagnostic(diagnostic_type::location_err)
                    .at(loc_peek())
                    .reason("Entry declaration is not allowed at namespace scope")
                    .report(this->diagnostics);
            }
            case tkn_type::kw_import:
            {
                skip();
                node->decls.push_back(parse_import());
                break;
            }
            case tkn_type::kw_extern:
            {
                skip();
                node->decls.push_back(parse_extern());
                break;
            }
            case tkn_type::kw_namespace:
            {
                skip();
                node->decls.push_back(parse_namespace());
                break;
            }
            case tkn_type::kw_func:
            {
                skip();
                node->decls.push_back(parse_func_decl());
                break;
            }
            case tkn_type::semi:
            {
                // Ignore null top-level declaration
                skip();
                break;
            }
            default:
            {
                this->ok = false;
                diagnostic(diagnostic_type::location_err)
                    .at(loc_peek())
                    .reason("Invalid declaration in namespace")
                    .report(this->diagnostics);
            }
        }
    }
    
    return node;
}

nodeh<ret_stmt> parser::parse_ret()
{
    auto node = new_node<ret_stmt>();
    node->loc = loc_peekb();
    node->val = parse_expr(true);
    expect(tkn_type::semi);
    node->loc.end = loc_peekb();
    return node;
}

nodeh<extern_decl> parser::parse_extern()
{
    auto node = new_node<extern_decl>();
    node->loc = loc_peekb();
    expect(tkn_type::literal);
    node->real_name = peekb().value;
    node->real_name.erase(0, 1);
    node->real_name.pop_back();
    node->loc.end = loc_peekb();
   
    if (match(tkn_type::kw_func))
    {
        node->inner_decl = parse_func_decl();
    }
    else
    {
        node->inner_decl = parse_var_decl();
    }
    
    node->loc.end = loc_peekb();
    return node;
}

stmth parser::parse_stmt()
{
    // Switches get stiches
    if (match(tkn_type::kw_ret))
    {
        return parse_ret();
    }
    else if (match(tkn_type::lbrace))
    {
        return parse_compound_stmt();
    }
    else if (match(tkn_type::kw_if))
    {
        return parse_cond();
    }
    else if (match(tkn_type::kw_while))
    {
        return parse_iter();
    }
    else if (is_var_decl())
    {
        return decl_stmt::from(parse_var_decl());
    }
    else if (match(tkn_type::semi))
    {
        // Null statement
        return new_node<null_stmt>(loc_peekb());
    }
    else if (auto expr = parse_expr(true))
    {
        // Hacky, but works :^)
        if (match(tkn_type::assign_op))
        {
            return parse_assign_stmt(std::move(expr));
        }
        expect(tkn_type::semi);
        return expr_stmt::from(std::move(expr));
    }
    return nullptr; // Error handled in expression
}

nodeh<compound_stmt> parser::parse_compound_stmt()
{
    auto node = new_node<compound_stmt>();
    node->loc = loc_peekb();

    while (this->ok && !match(tkn_type::rbrace) && !match(tkn_type::eof))
    {
        node->body.push_back(parse_stmt());
    }

    node->loc.end = loc_peekb();

    return node;
}
