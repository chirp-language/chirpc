#include "parser.hpp"

void parser::load_tokens(std::string fn, std::vector<token>&& t)
{
    this->filename = fn;
    this->tkns = std::move(t);
}

void parser::parse()
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
            this->tree.entry = get_entry();
            break;
        }
        case tkn_type::kw_import:
        {
            skip();
            this->tree.imports.push_back(get_import());
            break;
        }
        case tkn_type::kw_extern:
        {
            skip();
            this->tree.externs.push_back(get_extern());
            break;
        }
        case tkn_type::kw_func:
        {
            skip();
            auto f = get_func_decl();
            if (f->type == decl_type::fdef)
                this->tree.fdefs.push_back(std::shared_ptr<func_def>(std::move(f), static_cast<func_def*>(f.get())));
            else
                this->tree.fdecls.push_back(std::move(f));
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
            diagnostic e;
            e.type = diagnostic_type::location_err;
            e.l = loc_peek();
            e.msg = "Invalid top-level declaration";
            this->diagnostics.show(e);
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
    // Probably good enough to stop like 99% of bad behaviour
    if (!this->ok)
    {
        return false;
    }

    return this->peek().type == v;
}

bool parser::expect(tkn_type v)
{
    if (!match(v))
    {
        diagnostic e;
        e.type = diagnostic_type::location_err;
        if (cursor >= this->tkns.size())
        {
            e.l = loc_eof();
            e.msg = "Unexpected end of file.";
        }
        else
        {
            e.l = loc_peek();
            e.msg = "Unexpected token";
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
