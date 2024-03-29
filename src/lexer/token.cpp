#include "token.hpp"
#include <type_traits>

//#define tknstr(c)\
//    case tkn_type::c : result += #c;\
//    break;

#define tknstr(c) #c,

char const* token_names[] = {
    tknstr(kw_entry   )
    tknstr(kw_import  )
    tknstr(kw_export  )
    tknstr(kw_namespace)
    tknstr(kw_if      )
    tknstr(kw_else    )
    tknstr(kw_elif    )
    tknstr(kw_and     )
    tknstr(kw_or      )
    tknstr(kw_func    )
    tknstr(kw_while   )
    tknstr(kw_for     )
    tknstr(kw_ret     )
    tknstr(kw_extern  )
    tknstr(kw_true    )
    tknstr(kw_false   )
    tknstr(kw_null    )
    tknstr(kw_alloca  )
    tknstr(dt_int     )
    tknstr(dt_char    )
    tknstr(dt_float   )
    tknstr(dt_double  )
    tknstr(dt_byte    )
    tknstr(dt_bool    )
    tknstr(dt_long    )
    tknstr(dt_none    )
    tknstr(dm_ptr     )
    tknstr(dm_signed  )
    tknstr(dm_unsigned)
    tknstr(dm_const   )
    tknstr(period     )
    tknstr(colon      )
    tknstr(comma      )
    tknstr(assign_op  )
    tknstr(compassign_op)
    tknstr(lt_op      )
    tknstr(gt_op      )
    tknstr(lteq_op    )
    tknstr(gteq_op    )
    tknstr(eqeq_op    )
    tknstr(noteq_op   )
    tknstr(plus_op    )
    tknstr(minus_op   )
    tknstr(star_op    )
    tknstr(slash_op   )
    tknstr(perc_op    )
    tknstr(as_op      )
    tknstr(ref_op     )
    tknstr(deref_op   )
    tknstr(lparen     )
    tknstr(rparen     )
    tknstr(lbrace     )
    tknstr(rbrace     )
    tknstr(lbracket   )
    tknstr(rbracket   )
    tknstr(semi       )
    tknstr(identifer  )
    tknstr(literal    )
    tknstr(unknown    )
    tknstr(whitespace )
    tknstr(newline    )
    tknstr(eof        )
};

static_assert(std::size(token_names) == static_cast<int>(tkn_type::eof) + 1, "Size of token names array doesn't match number of tokens");

std::string token::util_dump(location_run* run)
{
    std::string result;

    result += token_names[static_cast<int>(this->type)];
    result += "   '";
    result += this->value;
    result += "' <";
    print_loc_single(loc, result, run);
    result += ">";
    return result;
}
