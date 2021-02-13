#include "parser.hpp"

#include <string.h>
#include <new>
#include <iostream>

bool parser::is_func_call()
{
    bool result = false;
    int og = this->cursor;
    if(match(identifer))
    {
        if(match(lparen)){
            result = true;
        }
        else if(match(period)){
            while(match(identifer)&&match(period)){}
            if(match(lparen)){
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
    while(match(identifer)){
        token ns = peekb();
        if(match(period)){
            node.namespaces.push_back(ns.value);
        }
        else{
            node.name = ns.value;
        }
    }
    return node;
}

txt_literal parser::get_txt_lit()
{
    txt_literal node;
    token t = peek();
    expect(literal);
    node.value = t.value;
    node.value.erase(0,1);
    node.value.pop_back();

    if(node.value.size() == 1){
        node.single_char = true;
    }
    else{
        node.single_char = false;
    }
    return node;
}

num_literal parser::get_num_lit()
{
    num_literal node;
    token t = peek();
    expect(literal);
    node.value = t.value;
    return node;
}

literal_node* parser::get_literal()
{
    literal_node* node;
    std::string val = peek().value;
    if(val.at(0) == '"' || val.at(0) == '\''){
        node = new txt_literal;
        *node = get_txt_lit();
    }
    // oof, doesn't check for booleans, too bad
    else{
        node = new num_literal;
        *node = get_num_lit();
    }
    return node;
}

arguments parser::get_arguments()
{
    arguments node;
    expect(lparen);
    while(!match(rparen)&&!match(eof)){
        node.body.push_back(get_literal());
    }
    return node; 
}

entry_stmt parser::get_entry()
{
    entry_stmt node;
    node.line = this->peek().loc.line;
    expect(kw_entry);
    node.code = get_stmt();
    return node;
}

import_stmt parser::get_import()
{
    import_stmt node;
    node.line = this->peek().loc.line;
    expect(kw_import);
    node.filename = get_txt_lit();
    return node;
}

ret_stmt parser::get_ret()
{
    ret_stmt node;
    node.line = this->peek().loc.line;
    expect(kw_ret);
    node.val = get_literal();
    return node;
}

func_call_stmt parser::get_fcall()
{
    func_call_stmt node;
    node.ident = get_identifier();
    node.args = get_arguments();
    return node;
}

stmt* parser::get_stmt()
{
    // Should probably be a std::unique_ptr tbh
    stmt* result = nullptr;
    tkn_type t = this->peek().type;
    // Switches get stiches
    if(t==kw_entry){
        result = new entry_stmt;
        *static_cast<entry_stmt*>(result) = get_entry();
    }
    else if(t==kw_import){
        result = new import_stmt;
        *static_cast<import_stmt*>(result) = get_import();
    }
    else if(t==kw_ret){
        result = new ret_stmt;
        *static_cast<ret_stmt*>(result) = get_ret();
    }
    else if(t==lbrace){
        result = new compound_stmt;
        *static_cast<compound_stmt*>(result) = get_compound_stmt();
    }
    else if(is_func_call()){
        result = new func_call_stmt;
        *static_cast<func_call_stmt*>(result) = get_fcall();
    }
    else{
        this->ok = false;

        helper e;
        e.type = location_err;
        e.l = this->peek().loc;
        e.msg = "Unexpected token in statement or something idk";
        
        this->helpers.push_back(e);
        result = nullptr;
    }
    return result;
}

compound_stmt parser::get_compound_stmt()
{
    compound_stmt node;
    node.line = peek().loc.line;
    expect(lbrace);
    while(this->ok && !match(rbrace) && !match(eof)){
        stmt* aaaa = get_stmt();
        node.body.push_back(aaaa);
    }
    return node;
}