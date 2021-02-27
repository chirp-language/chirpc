#pragma once

#include <string>
#include <vector>

/*
Semantics tracker, is used to get the states of variables
during codegen. The tracker is also used accross files,
so that means there is one tracker for multiple codegenerators,
which means it also tracks imports.
*/
class tracker
{
    public:
    // Doesn't really have any real use
    void init();

    

    // Returns true if success
    bool register_var();
    private:
    // Depth in the scope
    int depth;
};