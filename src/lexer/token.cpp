#include "token.hpp"

#define tknstr(c, alias)\
    case c : result += #alias;\
    break;

std::string token::util_dump(){
    std::string result;

    switch(this->type) {
        tknstr(tkn_type::kw_entry  , kw_entry  )
        tknstr(tkn_type::kw_import , kw_import )
        tknstr(tkn_type::kw_export , kw_export )
        tknstr(tkn_type::kw_if     , kw_if     )
        tknstr(tkn_type::kw_else   , kw_else   )
        tknstr(tkn_type::kw_elif   , kw_elif   )
        tknstr(tkn_type::kw_and    , kw_and    )
        tknstr(tkn_type::kw_or     , kw_or     )
        tknstr(tkn_type::kw_func   , kw_func   )
        tknstr(tkn_type::kw_while  , kw_while  )
        tknstr(tkn_type::kw_for    , kw_for    )
        tknstr(tkn_type::kw_ret    , kw_ret    )
        tknstr(tkn_type::kw_extern , kw_extern )
        tknstr(tkn_type::datatype  , datatype  )
        tknstr(tkn_type::datamod   , datamod   )
        tknstr(tkn_type::period    , period    )
        tknstr(tkn_type::colon     , colon     )
        tknstr(tkn_type::comma     , comma     )
        tknstr(tkn_type::assign_op , assign_op )
        tknstr(tkn_type::cmp_op    , cmp_op    )
        tknstr(tkn_type::math_op   , math_op   )
        tknstr(tkn_type::lparen    , lparen    )
        tknstr(tkn_type::rparen    , rparen    )
        tknstr(tkn_type::lbrace    , lbrace    )
        tknstr(tkn_type::rbrace    , rbrace    )
        tknstr(tkn_type::lbracket  , lbracket  )
        tknstr(tkn_type::rbracket  , rbracket  )
        tknstr(tkn_type::identifer , identifer )
        tknstr(tkn_type::literal   , literal   )
        tknstr(tkn_type::unknown   , unknown   )
        tknstr(tkn_type::whitespace, whitespace)
        tknstr(tkn_type::newline   , newline   )
        tknstr(tkn_type::eof       , eof       )
    }
    result += "   '";
    result += this->value;
	result += "' <";
	result += loc.filename;
	result += ":";
	if (loc.line == -1)
		result += "invalid";
	else
		result += std::to_string(loc.line+1);
	result += ":";
	if (loc.start == -1)
		result += "invalid";
	else
		result += std::to_string(loc.start+1);
    result += ">";
    return result;
}
