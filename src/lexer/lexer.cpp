#include "lexer.hpp"

#include <iostream>
#include <string_view>

static inline std::string_view token_value(std::vector<std::string> const& source, location const& loc)
{
    return std::string_view(source.at(loc.line)).substr(loc.start, loc.len);
}

static inline bool is_ident_begin(char c)
{
    return (c >= 'A' and c <= 'Z')
        or (c >= 'a' and c <= 'z')
        or c == '_';
}

static inline bool is_ident_char(char c)
{
    return is_ident_begin(c) or (c >= '0' and c <= '9');
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
            else if (is_ident_char(line.at(i)))
            {
                loc.start = i;
                loc.line = nline;
                ++i;
                while (i < line.size() && is_ident_char(line.at(i)))
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
                        diagnostic(diagnostic_type::location_err)
                            .at(token_location(i - 1))
                            .reason("Incomplete preprocessor directive")
                            .report(diagnostics);
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
                        diagnostic(diagnostic_type::location_warning)
                            .at(token_location(i - 1))
                            .reason("Unmached end directive")
                            .report(diagnostics);
                    }
                }
                else if (!raw_depth)
                {
                    diagnostic(diagnostic_type::location_err)
                        .at(token_location(i - 1))
                        .reason("Unknown preprocessor directive")
                        .report(diagnostics);
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


bool all_spaces(std::string const& txt)
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

bool is_number(std::string const& txt)
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

bool is_float(std::string const& txt)
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

bool is_addr(std::string const& txt)
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
        #define MATCH(v, s) \
            if (t.value == v)\
                t.type = s;\
            else
        #define MATCH_KW(v) MATCH(#v, tkn_type::kw_##v)
        MATCH_KW(entry)
        MATCH_KW(import)
        MATCH_KW(export)
        MATCH_KW(namespace)
        MATCH_KW(if)
        MATCH_KW(else)
        MATCH_KW(elif)
        MATCH_KW(and)
        MATCH_KW(or)
        MATCH_KW(func)
        MATCH_KW(while)
        MATCH_KW(for)
        MATCH_KW(ret)
        MATCH_KW(extern)
        MATCH_KW(true)
        MATCH_KW(false)
        MATCH_KW(null)
        MATCH_KW(alloca)
        #undef MATCH_KW
        // Types
        #define MATCH_DT(v) MATCH(#v, tkn_type::dt_##v)
        MATCH_DT(int)
        MATCH_DT(char)
        MATCH_DT(float)
        MATCH_DT(double)
        MATCH_DT(byte)
        MATCH_DT(bool)
        MATCH_DT(long)
        MATCH_DT(none)
        #undef MATCH_DT
        #define MATCH_DM(v) MATCH(#v, tkn_type::dm_##v)
        MATCH_DM(ptr)
        MATCH_DM(signed)
        MATCH_DM(unsigned)
        MATCH_DM(const)
        #undef MATCH_DM
        #undef MATCH
        // Symbols
        /*else*/ if (t.loc.len == 1)
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
                CASE('+', tkn_type::plus_op)
                CASE('-', tkn_type::minus_op)
                CASE('*', tkn_type::star_op)
                CASE('/', tkn_type::slash_op)
                CASE('%', tkn_type::perc_op)
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
                CASE('<', tkn_type::lteq_op)
                CASE('>', tkn_type::gteq_op)
                CASE('!', tkn_type::noteq_op)
                CASE('=', tkn_type::eqeq_op)
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
                is_addr(t.value))
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
