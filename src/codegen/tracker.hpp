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

    // If called and no entry exists, will set the entry_set flag
    // Returns false if an entry existed before
    // Returns true if no entry existed before
    bool request_entry();
    private:
    // Depth in the scope
    int depth;
    //
    bool entry_set;
};