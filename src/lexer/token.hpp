#pragma once
#include "../shared/location.hpp"
#include <string>

enum class tkn_type
{
    // KEYWORDS

    kw_entry, // entry
    kw_import, kw_export, // import export
    kw_namespace,
    kw_if, kw_else, kw_elif, // if else
    kw_and, kw_or,
    kw_func, // func
    kw_while, kw_for, // while for
    kw_ret, // ret
    kw_extern,

    // Tokens with multiple keywords 
    datatype, // int,char,float,double,bool..
    datamod, // ptr,signed,unsigned..
    
    // SYMBOLS

    period, // .
    colon, // :
    comma, // ,
    assign_op, // =
    cmp_op, // > < <= >= !=
    math_op, // + - * /
    ref_op, deref_op, as_op, // Should probably all be the same token (or all different)
    lparen, rparen, // ( )
    lbrace, rbrace, // { } 
    lbracket, rbracket, // [ ]
    semi, // ;

    
    // HELPER(best name I could)
    
    identifer,
    literal,
    unknown,
    whitespace,
    newline,
    eof // end of input
};

class token
{
    public:
    tkn_type type;
    std::string value;
    location loc;

    // Utility Function
    std::string util_dump();
};
