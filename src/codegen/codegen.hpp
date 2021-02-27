#pragma once
#include "tracker.hpp"
#include "../ast/ast.hpp"
#include "../shared/helper.hpp"
#include <string>
#include <vector>

class codegen
{
    public:
    void gen();

    void set_tree(ast);
    void set_tracker(tracker*);
    
    // The name is pretty bad ngl
    std::string get_result();
    private:
    
    std::string emit_compound(compound_stmt&);

    std::string emit_entry(entry_stmt&);
    void gen_toplevel();

    ast m_tree;
    tracker* m_tracker;
    std::string result;

    bool errored = false;
    std::vector<helper> helpers;
};