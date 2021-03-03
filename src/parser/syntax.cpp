#include "parser.hpp"

#include <string.h>
#include <new>
#include <iostream>

bool parser::is_identifier(bool reset)
{
    bool result = false;
    int op = this->cursor;

    do
    {
        if(match(tkn_type::identifer))
        {
            result = true;
        }
        else
        {
            result = false;
        }
    }
    while(match(tkn_type::comma));
    
    if(reset)
    {
        this->cursor = op;
    }

    return result;
}

identifier parser::get_identifier()
{
    identifier node;
    while (match(tkn_type::identifer))
    {
        token ns = peekb();
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
    return node;
}

txt_literal parser::get_txt_lit()
{
    txt_literal node;
    node.ltype = littype::txt;
    token t = peek();
    expect(tkn_type::literal);
    node.value = t.value;
    node.value.erase(0, 1);
    node.value.pop_back();

    if (node.value.size() == 1)
    {
        node.single_char = true;
    }
    else
    {
        node.single_char = false;
    }
    return node;
}

num_literal parser::get_num_lit()
{
    num_literal node;
    node.ltype = littype::num;
    token t = peek();
    expect(tkn_type::literal);
    
    if(t.value.at(0) == '\'' || t.value.at(0) == '"'){
        helper e;
        e.l = t.loc;
        e.msg = "Trying to perform math operation with a string literal";
        e.type = helper_type::location_err;
        this->ok = false;
        this->helpers.push_back(e);
    } // Carries on after that, as it shouldn't break anything, until the codegen phase, but it throws an error so it won't reach that
    node.value = t.value;
    return node;
}

std::shared_ptr<literal_node> parser::get_literal()
{
    std::shared_ptr<literal_node> node;
    std::string val = peek().value;

    if (val.at(0) == '"' || val.at(0) == '\'')
    {
        node = std::make_shared<txt_literal>();
        *static_cast<txt_literal*>(node.get()) = get_txt_lit();
    }
    // oof, doesn't check for booleans, too bad
    else
    {
        node = std::make_shared<num_literal>();;
        *static_cast<num_literal*>(node.get()) = get_num_lit();
    }
    return node;
}

entry_stmt parser::get_entry()
{
    entry_stmt node;
    node.type = stmt_type::entry;
    node.line = this->peek().loc.line;
    expect(tkn_type::kw_entry);
    node.code = get_stmt();
    return node;
}

import_stmt parser::get_import()
{
    import_stmt node;
    node.type = stmt_type::import;
    node.line = this->peek().loc.line;
    expect(tkn_type::kw_import);
    node.filename = get_txt_lit();
    return node;
}

ret_stmt parser::get_ret()
{
    ret_stmt node;
    node.type = stmt_type::ret;
    node.line = this->peek().loc.line;
    expect(tkn_type::kw_ret);
    node.val = get_expr();
    //node.val = get_literal();
    return node;
}

extern_stmt parser::get_extern()
{
    extern_stmt node;
    expect(tkn_type::kw_extern);
    node.real_name = get_txt_lit();
    node.type = 0;
    if(is_func_decl(true)){
        node.type = 1;
        node.stmt = std::make_shared<func_decl_stmt>(get_func_decl());
    }
    else if(is_var_decl(true)){
        node.type = 2;
        node.stmt = std::make_shared<decl_stmt>(get_decl_stmt());
    }
    return node;
}

std::shared_ptr<stmt> parser::get_stmt()
{
    // Should probably be a std::shared_ptr tbh
    std::shared_ptr<stmt> result;
    tkn_type t = this->peek().type;
    // Switches get stiches
    if (t == tkn_type::kw_ret)
    {
        result = std::make_shared<ret_stmt>();
        *static_cast<ret_stmt*>(result.get()) = get_ret();
    }
    else if (t == tkn_type::lbrace)
    {
        result = std::make_shared<compound_stmt>();
        *static_cast<compound_stmt*>(result.get()) = get_compound_stmt();
    }
    else if (is_func_call(true))
    {
        result = std::make_shared<func_call_stmt>();
        *static_cast<func_call_stmt*>(result.get()) = get_fcall();
    }
    else if(is_var_decldef())
    {
        result = std::make_shared<decldef_stmt>();
        *static_cast<decldef_stmt*>(result.get()) = get_decldef_stmt();
    }
    else if(is_var_decl(true))
    {
        result = std::make_shared<decl_stmt>();
        *static_cast<decl_stmt*>(result.get()) = get_decl_stmt();
    }
    else if(is_var_def(true))
    {
        result = std::make_shared<def_stmt>();
        *static_cast<def_stmt*>(result.get()) = get_def_stmt();
    }
    else
    {
        this->ok = false;

        helper e;
        e.type = helper_type::location_err;
        e.l = this->peek().loc;
        e.msg = "Statement could not be parsed";

        this->helpers.push_back(e);
        result = nullptr;
    }
    return result;
}

compound_stmt parser::get_compound_stmt()
{
    compound_stmt node;
    node.type = stmt_type::compound;
    node.line = peek().loc.line;
    expect(tkn_type::lbrace);
    while (this->ok && !match(tkn_type::rbrace) && !match(tkn_type::eof))
    {
        //stmt* aaaa = get_stmt();
        //node.body.push_back(aaaa);
        node.body.push_back(get_stmt());
    }
    return node;
}