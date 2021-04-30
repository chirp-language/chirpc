#include "lexer.hpp"

bool all_spaces(std::string txt)
{
    for (char c : txt)
    {
        if (isspace(c) == 0)
        {
            return false;
        }
    }
    return true;
}

bool is_number(std::string txt)
{
    for (char c : txt)
    {
        if (isdigit(c) == 0)
        {
            return false;
        }
    }
    return true;
}

bool is_float(std::string txt)
{
    if (txt.size() > 3)
    {
        if (isdigit(txt.at(0)) && txt.at(1) == '.' && txt.at(txt.size() - 1) == 'f')
        {
            return true;
        }
    }

    return false;
}

bool ishex(char c)
{
    if (
        isdigit(c) || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f')
    {
        return true;
    }
    return false;
}

bool is_addr(std::string txt)
{
    if (txt.size() > 3)
    {
        if (txt.at(0) == '0' && txt.at(1) && ishex(txt.at(txt.size() - 1)))
        {
            return true;
        }
    }

    return false;
}

std::vector<token> lexe(std::vector<location> src, std::vector<std::string> content)
{
    std::vector<token> result;

    for (location loc : src)
    {
        token t;
        t.loc = loc;
        if (t.loc.len == 0)
        {
            result.push_back(std::move(t));
            continue;
        }
        // String Views are a conspiracy
        for (int i = loc.start, end = i + loc.len; i < end; i++)
        {
            t.value += content.at(loc.line).at(i);
        }
        // Keywords
        if (t.value == "entry")
        {
            t.type = tkn_type::kw_entry;
        }
        else if (t.value == "import")
        {
            t.type = tkn_type::kw_import;
        }
        else if (t.value == "export")
        {
            t.type = tkn_type::kw_export;
        }
        else if (t.value == "if")
        {
            t.type = tkn_type::kw_if;
        }
        else if (t.value == "else")
        {
            t.type = tkn_type::kw_else;
        }
        else if(t.value == "and")
        {
            t.type = tkn_type::kw_and;
        }
        else if(t.value == "or")
        {
            t.type = tkn_type::kw_or;
        }
        else if (t.value == "func")
        {
            t.type = tkn_type::kw_func;
        }
        else if (t.value == "while")
        {
            t.type = tkn_type::kw_while;
        }
        else if (t.value == "for")
        {
            t.type = tkn_type::kw_for;
        }
        else if (t.value == "ret")
        {
            t.type = tkn_type::kw_ret;
        }
        else if (t.value == "extern")
        {
            t.type = tkn_type::kw_extern;
        }
        else if (
            t.value == "int"   || t.value == "char"   ||
            t.value == "float" || t.value == "double" ||
            t.value == "byte"  || t.value == "bool"   ||
            t.value == "none")
        {
            t.type = tkn_type::datatype;
        }
        else if (
            t.value == "ptr"      || t.value == "signed" ||
            t.value == "unsigned" || t.value == "const")
        {
            t.type = tkn_type::datamod;
        }
        // Symbols
        else if (t.value == ".")
        {
            t.type = tkn_type::period;
        }
        else if (t.value == ":")
        {
            t.type = tkn_type::colon;
        }
        else if (t.value == ",")
        {
            t.type = tkn_type::comma;
        }
        else if(t.value.size() == 2 && t.value.at(t.value.size() - 1) == '=')
        {
            char c = t.value.at(0);
            if(c=='<'||c=='>'||c=='!'||c=='=')
            {
                t.type = tkn_type::cmp_op;
            }
            else if(c=='+'||c=='-'||c=='*'||c=='/')
            {
                t.type = tkn_type::assign_op;
            }
            else
            {
                t.type = tkn_type::unknown;
            }
        }
        else if (t.value == "=")
        {
            t.type = tkn_type::assign_op;
        }
        else if (
            t.value == "+" || t.value == "-" || 
            t.value == "-" || t.value == "*" || 
            t.value == "/" )
        {
            t.type = tkn_type::math_op;
        }
        else if (t.value == "ref")
        {
            t.type = tkn_type::ref_op;
        }
        else if (t.value == "deref")
        {
            t.type = tkn_type::deref_op;
        }
        else if (t.value == "as")
        {
            t.type = tkn_type::as_op;
        }
        else if (t.value == "(")
        {
            t.type = tkn_type::lparen;
        }
        else if (t.value == ")")
        {
            t.type = tkn_type::rparen;
        }
        else if (t.value == "{")
        {
            t.type = tkn_type::lbrace;
        }
        else if (t.value == "}")
        {
            t.type = tkn_type::rbrace;
        }
        else if (t.value == "[")
        {
            t.type = tkn_type::lbracket;
        }
        else if (t.value == "]")
        {
            t.type = tkn_type::rbracket;
        }
        // Other
        else if (all_spaces(t.value))
        {
            t.type = tkn_type::whitespace;
        }
        else
        {
            if (
                t.value.at(0) == '"' || t.value.at(0) == '\'' || 
                is_number(t.value)   || is_float(t.value)     ||
                t.value == "true"    || t.value == "false"    || is_addr(t.value) )
            {
                t.type = tkn_type::literal;
            }
            else
            {
                t.type = tkn_type::identifer;
            }
        }
        result.push_back(std::move(t));
    }
    // Last token - EOF
    result.back().type = tkn_type::eof;

    return result;
}
