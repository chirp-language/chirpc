#pragma once
#include "../shared/location.hpp"
#include <string>

enum class tkn_type
{
    // KEYWORDS

    kw_entry, // entry
    kw_import,kw_export, // import export
    kw_if,kw_else, // if else
    kw_func, // func
    kw_while,kw_for, // while for
    kw_ret,

    // Tokens with multiple keywords 
    datatype, // int,char,float,double,bool..
    datamod, // ptr,signed,unsigned..
    
    // SYMBOLS

    period, // .
    colon, // :
    comma, // ,
    assign_op, // =
    math_op, // + - * /
    lparen,rparen, // ( )
    lbrace,rbrace, // { } 
    lbracket,rbracket, // [ ]

    
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