#include "lexer.hpp"

#include <iostream>
#include <string_view>

static inline std::string_view token_value(std::vector<std::string> const& source, location const& loc)
{
    return std::string_view(source.at(loc.line)).substr(loc.start, loc.len);
}

// Cuts the input into individual words/locations -> will later become tokens
std::vector<location> lexer::lex_raw()
{
    std::vector<location> result;

    size_t nline = 0; // line counter
    location loc;
    loc.filename = fname;
    for (std::string const& line : source)
    {
        for (size_t i = 0; i < line.size(); )
        {
            if (line.at(i) == '"')
            {
                loc.start = i;
                loc.line = nline;
                // Doesn't handle \"
                ++i;
                while (i < line.size() && line.at(i) != '"')
                {
                    // Skips escape characters
                    if (line.at(i) == '\\' && i + 1 < line.size())
                    {
                        ++i;
                    }
                    ++i;
                }
                ++i;
                loc.len = i - loc.start;
                result.push_back(loc);
            }
            else if (line.at(i) == '\'')
            {
                loc.start = i;
                loc.line = nline;
                // Doesn't handle \'
                ++i;
                while (i < line.size() && line.at(i) != '\'')
                {
                    // Skips escape characters
                    if (line.at(i) == '\\' && i + 1 < line.size())
                    {
                        ++i;
                    }
                    ++i;
                }
                ++i;
                loc.len = i - loc.start;
                result.push_back(loc);
            }
            else if (isalnum(line.at(i)))
            {
                loc.start = i;
                loc.line = nline;
                ++i;
                while (i < line.size() && isalnum(line.at(i)))
                {
                    if (i < line.size() - 2)
                    {
                        if (line.at(i + 1) == '.' && isdigit(line.at(i + 2)))
                        {
                            i += 2;
                        }
                    }
                    ++i;
                }
                loc.len = i - loc.start;
                result.push_back(loc);
            }
            else if (isspace(line.at(i)))
            {
                ++i;
                while (i < line.size() && isspace(line.at(i)) != 0)
                {
                    ++i;
                }
            }
            else if (line.at(i) == '#')
            {
                loc.start = i;
                loc.line = nline;
                i = line.size();
                loc.len = i - loc.start;
                result.push_back(loc);
            }
            else
            {
                loc.start = i;
                loc.line = nline;

                if(i + 1 < line.size() && line.at(i + 1) == '=')
                {
                    ++i;
                }

                ++i;

                loc.len = i - loc.start;
                result.push_back(loc);
            }
        }
        ++nline;
    }
    return result;
}

// This function should have no side-effects
std::vector<location> lexer::preprocess(std::vector<location> const& raw_tokens)
{
    enum class platform
    {
        WINDOWS,
        LINUX,
        BSD,
        OSX,
        UNIX,
        UNKNOWN
    };

    // Currently it only takes into account the system where it was compiled
    platform target_platform;

// Note: Linux should always be before the Unix macro
#if defined(_WIN64) || defined(_WIN32) || defined(__WINDOWS__)
    target_platform = platform::WINDOWS;
#elif defined(__linux) || defined(linux) || defined(__linux__)
    target_platform = platform::LINUX;
#elif defined(__DragonFly__) || defined(__FreeBSD)
    target_platform = platform::BSD;
#elif defined(__APPLE__) || defined(macintosh) || defined(__MACH__)
    target_platform = platform::OSX;
#elif defined(__unix) || defined(unix)
    target_platform = platform::UNIX;
#else
    target_platform = platform::UNKOWN;
#endif

    std::vector<location> result;
    size_t nline = -1;
    size_t raw_depth = 0;
    size_t nested_depth = 0;

    for (size_t i = 0; i < raw_tokens.size(); )
    {
        location const& loc = raw_tokens.at(i);
        bool begin_line = loc.line != nline;
        nline = loc.line;

        if (token_value(source, loc)[0] == '#')
        {
            ++i;
            while (i < raw_tokens.size() && nline == raw_tokens.at(i).line)
            {
                ++i;
            }
            continue;
        }
        if (token_value(source, loc) == "@")
        {
            // Preprocessing directive
            if (begin_line)
            {
                ++i;
                if (i == raw_tokens.size() || raw_tokens.at(i).line != nline)
                    continue;
                size_t end_dir;
                for (end_dir = i + 1; end_dir < raw_tokens.size(); ++end_dir)
                    if (raw_tokens.at(end_dir).line != nline)
                        break;

                auto cmd = token_value(source, raw_tokens.at(i));
                ++i;

                if (cmd == "platform")
                {
                    if (raw_depth)
                        continue;
                    if (i == end_dir)
                    {
                        diagnostic d;
                        d.type = diagnostic_type::location_err;
                        d.l = token_location(i - 1);
                        d.msg = "Incomplete preprocessor directive";
                        diagnostics.show(d);
                    }
                    else
                    {
                        auto pval = token_value(source, raw_tokens.at(i)); // platform name
                        ++i;
                        platform p;
                        if (pval == "windows")
                        {
                            p = platform::WINDOWS;
                        }
                        else if (pval == "linux")
                        {
                            p = platform::LINUX;
                        }
                        else if (pval == "bsd")
                        {
                            p = platform::BSD;
                        }
                        else if (pval == "apple" || pval == "osx" || pval == "mac")
                        {
                            p = platform::OSX;
                        }
                        else if (pval == "unix")
                        {
                            p = platform::UNIX;
                        }
                        else
                        {
                            p = platform::UNKNOWN;
                        }

                        if (target_platform == p)
                            ++nested_depth;
                        else
                            // Skip until an @end directive spotted
                            --nested_depth;
                    }
                }
                else if (cmd == "end")
                {
                    if (raw_depth != 0)
                    {
                        --raw_depth;
                    }
                    else if (nested_depth != 0)
                    {
                        --nested_depth;
                    }
                    else
                    {
                        diagnostic d;
                        d.type = diagnostic_type::location_warning;
                        d.l = token_location(i - 1);
                        d.msg = "Unmached end directive";
                        diagnostics.show(d);
                    }
                }
                else if (!raw_depth)
                {
                    diagnostic d;
                    d.type = diagnostic_type::location_err;
                    d.l = token_location(i - 1);
                    d.msg = "Unknown preprocessor directive";
                    diagnostics.show(d);
                }
            }
        }
        else
        {
            if (!raw_depth)
                result.push_back(raw_tokens.at(i));
            ++i;
        }
    }

    // EOF
    result.push_back(location(source.size(), fname));

    return result;
}


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

std::vector<token> lexer::lex(std::vector<location> const& src)
{
    std::vector<token> result;

    for (location loc : src)
    {
        token t;
        t.loc = loc;
        t.type = tkn_type::unknown;
        if (t.loc.len == 0)
        {
            result.push_back(std::move(t));
            continue;
        }
        t.value = token_value(source, loc);

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
        else if (t.value == "elif")
        {
            t.type = tkn_type::kw_elif;
        }
        else if (t.value == "and")
        {
            t.type = tkn_type::kw_and;
        }
        else if (t.value == "or")
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
        else if (t.loc.len == 1)
        {
            switch (t.value.at(0))
            {
                #define CASE(c, s) \
                case c:\
                    t.type = s;\
                    break;
                CASE('.', tkn_type::period)
                CASE(':', tkn_type::colon)
                CASE(',', tkn_type::comma)
                CASE('=', tkn_type::assign_op)
                CASE('+', tkn_type::math_op)
                CASE('-', tkn_type::math_op)
                CASE('*', tkn_type::math_op)
                CASE('/', tkn_type::math_op)
                CASE('%', tkn_type::math_op)
                CASE('(', tkn_type::lparen)
                CASE(')', tkn_type::rparen)
                CASE('{', tkn_type::lbrace)
                CASE('}', tkn_type::rbrace)
                CASE('[', tkn_type::lbracket)
                CASE(']', tkn_type::rbracket)
                CASE(';', tkn_type::semi)
                default:
                    goto L_lex_ident;
            }
        }
        else if (t.value.size() == 2 && t.value.at(1) == '=')
        {
            switch (t.value.at(0))
            {
                CASE('<', tkn_type::cmp_op)
                CASE('>', tkn_type::cmp_op)
                CASE('!', tkn_type::cmp_op)
                CASE('=', tkn_type::cmp_op)
                CASE('+', tkn_type::assign_op)
                CASE('-', tkn_type::assign_op)
                CASE('*', tkn_type::assign_op)
                CASE('/', tkn_type::assign_op)
                #undef CASE
            }
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
        
        // Other
        else if (all_spaces(t.value))
        {
            t.type = tkn_type::whitespace;
        }
        else
        {
            L_lex_ident:
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
