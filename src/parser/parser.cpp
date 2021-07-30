#include "parser.hpp"

void parser::load_tokens(std::string fn, std::vector<token>&& t)
{
    this->filename = fn;
    this->tkns = std::move(t);
}

void parser::parse_top_level()
{
    this->ok = true;

    // Only gets the top-level stuff
    while(this->ok and !match(tkn_type::eof))
    {
        switch (peek().type)
        {
        case tkn_type::kw_entry:
        {
            skip();
            if (this->tree.entry) {
                this->ok = false;
                diagnostic(diagnostic_type::location_err)
                    .at(loc_peekb())
                    .reason("Multiple definitions of entry point")
                    .report(this->diagnostics);
            }
            auto entry = parse_entry();
            this->tree.entry = entry.get();
            this->tree.top_decls.push_back(std::move(entry));
            break;
        }
        case tkn_type::kw_import:
        {
            skip();
            this->tree.top_decls.push_back(parse_import());
            break;
        }
        case tkn_type::kw_extern:
        {
            skip();
            this->tree.top_decls.push_back(parse_extern());
            break;
        }
        case tkn_type::kw_namespace:
        {
            skip();
            this->tree.top_decls.push_back(parse_namespace());
            break;
        }
        case tkn_type::kw_func:
        {
            skip();
            this->tree.top_decls.push_back(parse_func_decl());
            break;
        }
        case tkn_type::semi:
        {
            // Ignore null top-level declaration
            skip();
            break;
        }
        default:
        {
            this->ok = false;
            diagnostic(diagnostic_type::location_err)
                .at(loc_peek())
                .reason("Invalid top-level declaration")
                .report(this->diagnostics);
        }
        }
    }
}

bool parser::match(tkn_type v)
{
    // Probably good enough to stop like 99% of bad behaviour
    if (!this->ok)
    {
        return false;
    }

    if (this->peek().type == v)
    {
        cursor++;
        return true;
    }
    return false;
}

bool parser::probe(tkn_type v)
{
    return peek().type == v;
}

bool parser::probe_range(tkn_type begin, tkn_type end)
{
    return peek().type >= begin and peek().type <= end;
}

bool parser::expect(tkn_type v)
{
    if (!match(v))
    {
        diagnostic e;
        e.type = diagnostic_type::location_err;
        if (cursor >= this->tkns.size())
        {
            e.loc = loc_eof();
            e.msg = "Unexpected end of file.";
        }
        else
        {
            e.loc = loc_peek();
            e.msg = "Unexpected token, expected: ";
            e.msg += token_names[static_cast<int>(v)];
        }
        this->diagnostics.show(e);
        return false;
    }
    return true;
}

token const& parser::peekb()
{
    if (cursor > 0)
    {
        return this->tkns.at(cursor - 1);
    }
    else
    {
        return this->tkns.back();
    }
}

token const& parser::peek()
{
    if (cursor >= 0 && cursor < this->tkns.size())
    {
        return this->tkns.at(cursor);
    }
    else
    {
        // Assume last token is eof (which it should!)
        return this->tkns.back();
    }
}

token const& parser::peekf()
{
    if (cursor + 1 < this->tkns.size())
    {
        return this->tkns.at(cursor + 1);
    }
    else
    {
        return this->tkns.back();
    }
}

location const& parser::get_loc(token_location loc) const
{
    static location const s_invalid("<invalid>");
    if (loc.is_valid())
        return this->tkns.at(loc.loc).loc;
    return s_invalid;
}
