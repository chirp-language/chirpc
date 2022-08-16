#include "cache.hpp"

#include <iostream>
#include <fstream>

bool cache::write_to_file(std::string filename)
{
    std::cout<<"writing to "<<filename<<std::endl;
    std::ofstream f;

    f.open(filename);

    if(!f)
    {
        std::cout<<"Couldn't open file "<<filename<<std::endl;
    }
    
    for(std::pair<std::string, bonk::value> x : this->data)
    {
        std::string r = bonk::to_string(x.first, x.second);
        f << r;
        std::cout<<r<<std::endl;
    }

    f.close();
    return true;
}

void cache::keep_symbol(tracker_symbol& t)
{
    std::cout<<"Keeping "<<t.name.name<<std::endl;
}

void cache::keep_namespace(const namespace_decl& e, std::vector<std::pair<std::string,std::string>> decls)
{
    bonk::value val;
    val.type = 2;
    val.data = new std::map<std::string, bonk::value>;
    std::cout<<"Keeping "<<e.ident.name<<std::endl;
    std::map<std::string, bonk::value> bdecls; //decl
    
    static_cast<std::map<std::string, bonk::value>*>(val.data)->insert(std::make_pair("type",bonk::make_string("namespace")));

    bonk::value v2;

    v2 = val;

    auto p  = std::make_pair(e.ident.name, val);

    this->data.push_back(p);
}