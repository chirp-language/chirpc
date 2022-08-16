#pragma once

#include <string>
#include "../bonk/bonk.hpp"
#include "../seman/tracker.hpp"

class cache
{
    public:
    void gen();

    bool write_to_file(std::string);

    void keep_symbol(tracker_symbol&);
    void keep_namespace(const namespace_decl&, std::vector<std::pair<std::string,std::string>>);

    private:
    std::vector<std::pair<std::string, bonk::value>> data;
};