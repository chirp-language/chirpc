#include "token.hpp"

#define tknstr(c) case c:result+=#c;break;

std::string token::util_dump(){
    std::string result;
    switch(this->type){
        tknstr(kw_entry)
        tknstr(kw_import)
        tknstr(kw_export)
        tknstr(kw_if)
        tknstr(kw_else)
        tknstr(kw_func)
        tknstr(kw_while)
        tknstr(kw_for)
        tknstr(kw_ret)
        tknstr(period)
        tknstr(colon)
        tknstr(comma)
        tknstr(assign_op)
        tknstr(math_op)
        tknstr(lparen)
        tknstr(rparen)
        tknstr(lbrace)
        tknstr(rbrace)
        tknstr(lbracket)
        tknstr(rbracket)
        tknstr(identifer)
        tknstr(literal)
        tknstr(unknown)
        tknstr(whitespace)
        tknstr(newline)
        tknstr(eof)
    }
    result += "   '";
    result += this->value;
    result += "' Loc<";
    result += loc.filename;
    result += ":";
    result += std::to_string(loc.start);
    result += ":";
    result += std::to_string(loc.line);
    result += ">";
    return result;
}