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

std::shared_ptr<literal_node> parser::get_literal()
{
    auto const& val = peek().value;

    if (val.at(0) == '"' || val.at(0) == '\'')
        return std::make_shared<txt_literal>(get_txt_lit());
    // oof, doesn't check for booleans, too bad
    else
        return std::make_shared<num_literal>(get_num_lit());
    return nullptr;
}

std::shared_ptr<entry_decl> parser::get_entry()
{
    auto node = std::make_shared<entry_decl>();
    node->loc = loc_peekb();
    expect(tkn_type::lbrace);
    node->code = get_compound_stmt();
    node->loc.end = loc_peekb();
    return node;
}

std::shared_ptr<import_decl> parser::get_import()
{
    auto node = std::make_shared<import_decl>();
    node->loc = loc_peekb();
    node->filename = get_txt_lit();
    node->loc = loc_peekb();
    return node;
}

std::shared_ptr<ret_stmt> parser::get_ret()
{
    auto node = std::make_shared<ret_stmt>();
    node->loc = loc_peekb();
    node->val = get_expr(true);
    expect(tkn_type::semi);
    node->loc.end = loc_peekb();
    return node;
}

std::shared_ptr<extern_decl> parser::get_extern()
{
    auto node = std::make_shared<extern_decl>();
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
        return std::make_shared<null_stmt>(loc_peekb());
    }
    else if (auto expr = get_expr(true))
    {
        expect(tkn_type::semi);
        return expr_stmt::from(std::move(expr));
    }
    else
    {
        this->ok = false;

        diagnostic e;
        e.type = diagnostic_type::location_err;
        e.l = loc_peek();
        e.msg = "Statement could not be parsed";

        this->diagnostics.show(e);
        return nullptr;
    }
}

std::shared_ptr<compound_stmt> parser::get_compound_stmt()
{
    auto node = std::make_shared<compound_stmt>();
    node->loc = loc_peekb();

    while (this->ok && !match(tkn_type::rbrace) && !match(tkn_type::eof))
    {
        node->body.push_back(get_stmt());
    }

    node->loc.end = loc_peekb();

    return node;
}
