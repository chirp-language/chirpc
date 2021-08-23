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
    kw_and, kw_or, // and or (should be operators)
    kw_func, // func
    kw_while, kw_for, // while for
    kw_ret, // ret
    kw_extern, // extern
    kw_true, // true
    kw_false, // false
    kw_null, // null
    // {:>>
    kw_alloca, // alloca

    // Tokens with multiple keywords
    // Types
    dt_int,
    dt_char,
    dt_float,
    dt_double,
    dt_byte,
    dt_bool,
    dt_long,
    dt_none,
    datatype_S = dt_int,
    datatype_E = dt_none,
    dm_ptr,
    dm_signed,
    dm_unsigned,
    dm_const,
    datamod_S = dm_ptr,
    datamod_E = dm_const,

    // SYMBOLS

    period, // .
    colon, // :
    comma, // ,
    assign_op, // =
    compassign_op, // #= where the next token corresponds to the operation
    lt_op, gt_op, lteq_op, gteq_op, eqeq_op, noteq_op, // > < <= >= == !=
    plus_op, minus_op, star_op, slash_op, perc_op, // + - * / %
    as_op, // as
    cmp_S = lt_op,
    cmd_E = noteq_op,
    binop_S = lt_op,
    binop_E = as_op,
    ref_op, deref_op, // ref deref (unary)
    // + & - can be unary
    lparen, rparen, // ( )
    lbrace, rbrace, // { }
    lbracket, rbracket, // [ ]
    semi, // ;


    // MISC
    
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

std::string exprop_id(tkn_type op);
extern char const* token_names[];
