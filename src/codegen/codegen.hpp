#pragma once

#include <string>

#include "../ast/ast.hpp"
#include "../tracker/tracker.hpp"

class codegen
{
    public:
    void gen();

    void set_tree(ast);
    void set_tracker(tracker*);
    
    // The name is pretty bad ngl
    std::string get_result();
    private:
    ast m_tree;
    tracker* m_tracker;
    std::string result;
};