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
}

bool parser::match(tkn_type v)
{
    if(this->peek().type == v){
        cursor++;
        return true;
    }
    return false;
}

bool parser::expect(tkn_type v)
{
    if(!match(v)){
        helper e;
        if(cursor >= this->tkns.size()){
            e.type = line_err;
            // If line err, doesn't care about start or end, just line
            location l;
            l.filename = this->filename;
            l.line = this->tkns.at(this->tkns.size()-1).loc.line;
            e.msg = "Unexpected end of file.";
        }
        else{
            e.l = tkns.at(this->cursor).loc;
            e.l.filename = this->filename;
            e.type = location_err;
            e.msg = "Unexpected token";
        }
        this->helpers.push_back(e);
    }
    return true;
}

token parser::peekb()
{
    if(cursor > 0){
        return this->tkns.at(cursor-1);
    }
    else{
        token t;
        t.type = eof;
        return t;
    }
}

token parser::peek()
{
    if(cursor >= 0 && cursor < this->tkns.size()){
        return this->tkns.at(cursor);
    }
    else{
        token t;
        t.type = eof;
        return t;
    }
}

token parser::peekf()
{
    if(cursor + 1 < this->tkns.size()){
        return this->tkns.at(cursor+1);
    }
    else{
        token t;
        t.type = eof;
        return t;
    }
}