#include "value.hpp"

#include <map>
#include <string>

#include <iostream>

bonk::value::value()
{
    this->type = 1;
    this->data = nullptr;
}

bonk::value::value(int t, void* d)
{
    this->type = t;
    this->data = d;
}

bonk::value::value(const value& value2)
{
    this->type = value2.type;
    
    if(type == 3)
    {
        this->data = new int;
        *static_cast<int*>(this->data) = *static_cast<int*>(value2.data);
    }
    else if(type == 4)
    {
        this->data = new std::string;
        *static_cast<std::string*>(this->data) = *static_cast<std::string*>(value2.data);
    }
    else if(type == 5)
    {
        this->data = new bool;
        *static_cast<bool*>(this->data)  = *static_cast<bool*>(value2.data);
    }
    else
    {
        this->data = nullptr;
    }
}

bonk::value::~value()
{
    if(type == 0)
    {
    }
}

std::ostream& bonk::operator<<(std::ostream& os, const bonk::value& v)
{
    if(v.type == 0)
    {
        os << std::string("invalid");
    }
    else if(v.type == 1)
    {
        os << std::string("none");
    }
    else if(v.type  == 2)
    {
        os << std::string("list (")<<std::to_string(static_cast<std::map<std::string, bonk::value>*>(v.data)->size())<<std::string(")");
    }
    else if(v.type == 3)
    {
        os << std::to_string(*static_cast<int*>(v.data));
    }
    else if(v.type == 4)
    {
        os << *static_cast<std::string*>(v.data);
    }
    else if(v.type == 5)
    {
        bool x = *static_cast<bool*>(v.data);

        if(x)
        {
            os << std::string("true");
        }
        else
        {
            os << std::string("false");
        }
    }
    return os;
}