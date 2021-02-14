#include "parser.hpp"

#include <string.h>
#include <new>
#include <iostream>

bool parser::is_func_call()
{
    bool result = false;
    int og = this->cursor;

    if (match(tkn_type::identifer))
    {
        if (match(tkn_type::lparen))
        {
            result = true;
        }
        else if (match(tkn_type::period))
        {
            while (match(tkn_type::identifer) && match(tkn_type::period));

            if (match(tkn_type::lparen))
            {
                result = true;
            }
        }
    }

    this->cursor = og;
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
        }
    }
    return node;
}

txt_literal parser::get_txt_lit()
{
    txt_literal node;
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

arguments parser::get_arguments()
{
    arguments node;
    expect(tkn_type::lparen);
    
    while (!match(tkn_type::rparen) && !match(tkn_type::eof) && this->ok)
    {
        match(tkn_type::comma);
        node.body.push_back(get_expr());
    }
    
    return node;
}

entry_stmt parser::get_entry()
{
    entry_stmt node;
    node.line = this->peek().loc.line;
    expect(tkn_type::kw_entry);
    node.has_args = false;

    // Then the entry has arguments
    if(peek().type == tkn_type::lparen)
    {
        node.has_args = true;
        node.args = std::make_shared<arguments>(get_arguments());
    }
    else{
        node.args = std::shared_ptr<arguments>(nullptr);
    }

    node.code = get_stmt();
    return node;
}

import_stmt parser::get_import()
{
    import_stmt node;
    node.line = this->peek().loc.line;
    expect(tkn_type::kw_import);
    node.filename = get_txt_lit();
    return node;
}

ret_stmt parser::get_ret()
{
    ret_stmt node;
    node.line = this->peek().loc.line;
    expect(tkn_type::kw_ret);
    node.val = get_expr();
    //node.val = get_literal();
    return node;
}

func_call_stmt parser::get_fcall()
{
    func_call_stmt node;
    node.ident = get_identifier();
    node.args = get_arguments();
    return node;
}

std::shared_ptr<stmt> parser::get_stmt()
{
    // Should probably be a std::shared_ptr tbh
    std::shared_ptr<stmt> result;
    tkn_type t = this->peek().type;
    // Switches get stiches
    if (t == tkn_type::kw_entry)
    {
        result = std::make_shared<entry_stmt>();
        *static_cast<entry_stmt*>(result.get()) = get_entry();
    }
    else if (t == tkn_type::kw_import)
    {
        result = std::make_shared<import_stmt>();
        *static_cast<import_stmt*>(result.get()) = get_import();
    }
    else if (t == tkn_type::kw_ret)
    {
        result = std::make_shared<ret_stmt>();
        *static_cast<ret_stmt*>(result.get()) = get_ret();
    }
    else if (t == tkn_type::lbrace)
    {
        result = std::make_shared<compound_stmt>();
        *static_cast<compound_stmt*>(result.get()) = get_compound_stmt();
    }
    else if (is_func_call())
    {
        result = std::make_shared<func_call_stmt>();
        *static_cast<func_call_stmt*>(result.get()) = get_fcall();
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