#include "tracker.hpp"

void tracker::init()
{
    this->depth = 0;
    this->entry_set = false;
}

bool tracker::register_var(std::vector<std::string> nspace,std::string name)
{
    if(!this->check_var(nspace,name))
    {
        tracked_var v;
        v.namespaces = nspace;
        v.ident = name;
        v.depth = this->depth;
        this->vars.push_back(v);
        return true;
    }
    return false;
}

// Linear Search, inneficient. 
bool tracker::check_var(std::vector<std::string> nspace, std::string name)
{
    bool result = false;
    bool found = false;
    for(tracked_var var : this->vars)
    {
        if(var.ident == name)
        {
            if(!found)
            {
                result = true;
                found = true;
            }
            else
            {
                result = false;
            }
        }
    }
    return result;
}

// This function is incredibly inneficient
void tracker::scope_up()
{
    this->depth--;

    std::vector<tracked_var> nvec; // new vector
    for(tracked_var& var : this->vars)
    {
        if(var.depth <= this->depth)
        {
            nvec.push_back(var);
        }
    }
    this->vars = nvec;
}

void tracker::scope_down()
{
    this->depth++;
}

bool tracker::request_entry()
{
    if(this->entry_set)
    {
        return false;
    }
    else
    {
        this->entry_set = true;
        return true;
    }
}