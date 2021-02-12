#pragma once
#include "location.hpp"
#include <string>

enum tkn_type
{
    // KEYWORDS

    kw_entry, // entry
    kw_import,kw_export, // import export
    kw_if,kw_else, // if else
    kw_func, // func
    kw_while,kw_for, // while for
    
    // SYMBOLS

    period, // .
    colon, // :
    comma, // ,
    assign_op, // =
    math_op, // + - * /

    
    // HELPER(best name I could)

    whitespace,
    newline,
    end // end of input
};

class token
{
    public:
    tkn_type type;
    std::string value;
    location loc;
};