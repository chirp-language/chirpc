#include "parser.hpp"

#include <string.h>
#include <new>
#include <iostream>
#include <string>

txt_literal parser::build_txt_lit(token_location loc, std::string&& value, bool is_character)
{
    txt_literal node;
    node.loc = loc;
    node.value = std::move(value);
    node.is_character = is_character;
    if (is_character and node.value.size() != 1)
    {
        diagnostic(diagnostic_type::location_warning)
            .at(loc)
            .reason("Character value is not single-character")
            .report(this->diagnostics);
    }
    return node;
}

num_literal parser::build_num_lit(token_location loc, std::string const& value)
{
    num_literal node;
    node.type.basetp = dtypename::_int;
    node.cat = exprcat::rval;
    node.loc = loc;
    node.value = value;
    return node;
}

num_literal parser::build_bool_lit(token_location loc, bool value)
{
    num_literal node;
    node.type.basetp = dtypename::_bool;
    node.cat = exprcat::rval;
    node.loc = loc;
    node.value = value ? "1" : "0";
    return node;
}

num_literal parser::build_null_ptr_lit(token_location loc)
{
    num_literal node;
    node.type.basetp = dtypename::_none;
    node.type.exttp.push_back(static_cast<std::byte>(dtypemod::_ptr));
    node.cat = exprcat::rval;
    node.loc = loc;
    node.value = "null";
    return node;
}

nodeh<txt_literal> parser::parse_txt_lit(token_location loc, std::string const& tok_value)
{
    bool is_character = tok_value.at(0) == '\'';
    std::string value(tok_value);
    value.pop_back();
    value.erase(0, 1);
    return new_node<txt_literal>(build_txt_lit(loc, std::move(value), is_character));
}

exprh parser::parse_literal()
{
    auto const& val = peek().value;
    token_location loc = loc_peek();
    skip();

    if (val.at(0) == '"' || val.at(0) == '\'')
        return parse_txt_lit(loc, val);
    else
        return new_node<num_literal>(build_num_lit(loc, val));
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
