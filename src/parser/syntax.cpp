#include "parser.hpp"

#include <string.h>
#include <new>
#include <iostream>

txt_literal parser::get_txt_lit()
{
    txt_literal node;
    token const& t = peek();
    node.loc = loc_peek();
    expect(tkn_type::literal);
    node.value = t.value;
    node.value.erase(0, 1);
    node.value.pop_back();

    node.is_character = node.value.size() == 1;
    return node;
}

num_literal parser::get_num_lit()
{
    num_literal node;
    node.type.basetp = dtypename::_int;
    node.cat = exprcat::rval;
    token const& t = peek();
    node.loc = loc_peek();
    expect(tkn_type::literal);

    if (t.value.at(0) == '\'' || t.value.at(0) == '"')
    {
        diagnostic e;
        e.l = node.loc;
        e.msg = "Trying to perform math operation with a string literal";
        e.type = diagnostic_type::location_err;
        this->ok = false;
        this->diagnostics.show(e);
    } 
    // Carries on after that, as it shouldn't break anything, 
    // until the codegen phase, but it throws an error so it won't reach that
    node.value = t.value;
    return node;
}

num_literal parser::get_bool_lit()
{
    num_literal node;
    node.type.basetp = dtypename::_bool;
    node.cat = exprcat::rval;
    node.loc = loc_peek();
    node.value = probe(tkn_type::kw_true) ? "1" : "0";
    skip();
    return node;
}

exprh parser::get_literal()
{
    auto const& val = peek().value;

    if (val.at(0) == '"' || val.at(0) == '\'')
        return new_node<txt_literal>(get_txt_lit());
    else
        return new_node<num_literal>(get_num_lit());
    return nullptr;
}

nodeh<entry_decl> parser::get_entry()
{
    auto node = new_node<entry_decl>();
    node->loc = loc_peekb();
    expect(tkn_type::lbrace);
    node->code = get_compound_stmt();
    node->loc.end = loc_peekb();
    return node;
}

nodeh<import_decl> parser::get_import()
{
    auto node = new_node<import_decl>();
    node->loc = loc_peekb();
    node->filename = get_txt_lit();
    node->loc = loc_peekb();
    return node;
}

nodeh<ret_stmt> parser::get_ret()
{
    auto node = new_node<ret_stmt>();
    node->loc = loc_peekb();
    node->val = get_expr(true);
    expect(tkn_type::semi);
    node->loc.end = loc_peekb();
    return node;
}

nodeh<extern_decl> parser::get_extern()
{
    auto node = new_node<extern_decl>();
    node->loc = loc_peekb();
    node->real_name = get_txt_lit();
   
    if (match(tkn_type::kw_func))
    {
        node->inner_decl = get_func_decl();
    }
    else
    {
        node->inner_decl = get_var_decl();
    }
    node->loc.end = loc_peekb();
    return node;
}

stmth parser::get_stmt()
{
    // Switches get stiches
    if (match(tkn_type::kw_ret))
    {
        return get_ret();
    }
    else if (match(tkn_type::lbrace))
    {
        return get_compound_stmt();
    }
    else if (match(tkn_type::kw_if))
    {
        return get_cond();
    }
    else if (match(tkn_type::kw_while))
    {
        return get_iter();
    }
    else if (is_var_decl())
    {
        return decl_stmt::from(get_var_decl());
    }
    else if (is_var_assign())
    {
        return get_assign_stmt();
    }
    else if (match(tkn_type::semi))
    {
        // Null statement
        return new_node<null_stmt>(loc_peekb());
    }
    else if (auto expr = get_expr(true))
    {
        expect(tkn_type::semi);
        return expr_stmt::from(std::move(expr));
    }
    return nullptr; // Error handled in expression
}

nodeh<compound_stmt> parser::get_compound_stmt()
{
    auto node = new_node<compound_stmt>();
    node->loc = loc_peekb();

    while (this->ok && !match(tkn_type::rbrace) && !match(tkn_type::eof))
    {
        node->body.push_back(get_stmt());
    }

    node->loc.end = loc_peekb();

    return node;
}
