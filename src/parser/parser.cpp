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
    while (!match(tkn_type::eof) && !this->should_quit)
    {
        this->tree.root.children.push_back(this->get_stmt());

        // Ignores the rest of the tokens until next line/statement
        if(!this->ok && !this->should_quit)
        {
            while( this->cursor<this->tkns.size()&&
            (!match(tkn_type::semicolon)&&!match(tkn_type::newline)) ){
                this->cursor++;
            }
        }
        else
        {
            match(tkn_type::newline);
            match(tkn_type::semicolon);
            match(tkn_type::newline);
        }
    }
}

// Note: Will ignore newlines, unless specifically checking for newline
bool parser::match(tkn_type v)
{
    // Probably good enough to stop like 99% of bad behaviour
    if (!this->ok && !(v == tkn_type::eof || v == tkn_type::newline))
    {
        return false;
    }

    if(this->peek().type == tkn_type::newline && v != tkn_type::newline)
    {
        cursor++;
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
            this->should_quit = true;
        }
        else
        {
            e.l = tkns.at(this->cursor).loc;
            e.l.filename = this->filename;
            e.type = helper_type::location_err;
            e.msg = "Unexpected token";
        }
        this->ok = false;
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