#include "parser.hpp"

void parser::load_tokens(std::string fn, std::vector<token> t)
{
    this->filename = fn;
    this->tkns = t;
}

std::vector<helper> parser::get_helpers()
{
    return this->helpers;
}

ast parser::get_ast()
{
    return this->tree;
}

void parser::parse()
{
    this->ok = true;
    //this->tree.root.children.push_back(this->get_stmt());
    // Only gets the top-level stuff
    while(!match(tkn_type::eof) && this->ok)
    {
        tkn_type t = this->peek().type;

        if(t == tkn_type::kw_entry)
        {
            this->tree.entry = get_entry();
            this->tree.has_entry = true;
        }
        else if(t == tkn_type::kw_import)
        {
            this->tree.imports.push_back(get_import());
        }
        else if(t == tkn_type::kw_extern)
        {
            this->tree.externs.push_back(get_extern());
        }
        else if(t == tkn_type::kw_func)
        {
            if(is_func_def(true))
            {
                this->tree.fdefs.push_back(get_func_def());
            }
            else if(is_func_decl(true))
            {
                this->tree.fdecls.push_back(get_func_decl());
            }
            else
            {
                // Is either a var or an error
                helper e;
                e.l = peek().loc;
                e.msg = "I am a lazy person";
                e.type = helper_type::location_err;
                this->helpers.push_back(e);
                this->ok = false;
            }
        }
        else
        {
            this->ok = false;
            helper e;
            e.type = helper_type::location_err;
            e.l = this->peek().loc;
            e.msg = "Invalid top-level statement";
            this->helpers.push_back(e);
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

bool parser::expect(tkn_type v)
{
    if (!match(v))
    {
        helper e;
        if (cursor >= this->tkns.size())
        {
            e.type = helper_type::line_err;
            // If line err, doesn't care about start or end, just line
            location l;
            l.filename = this->filename;
            l.line = this->tkns.at(this->tkns.size() - 1).loc.line;
            e.msg = "Unexpected end of file.";
        }
        else
        {
            e.l = tkns.at(this->cursor).loc;
            e.l.filename = this->filename;
            e.type = helper_type::location_err;
            e.msg = "Unexpected token";
        }
        this->helpers.push_back(e);
    }
    return true;
}

token parser::peekb()
{
    if (cursor > 0)
    {
        return this->tkns.at(cursor - 1);
    }
    else
    {
        token t;
        t.type = tkn_type::eof;
        return t;
    }
}

token parser::peek()
{
    if (cursor >= 0 && cursor < this->tkns.size())
    {
        return this->tkns.at(cursor);
    }
    else
    {
        token t;
        t.type = tkn_type::eof;
        return t;
    }
}

token parser::peekf()
{
    if (cursor + 1 < this->tkns.size())
    {
        return this->tkns.at(cursor + 1);
    }
    else
    {
        token t;
        t.type = tkn_type::eof;
        return t;
    }
}