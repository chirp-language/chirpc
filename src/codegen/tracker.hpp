#pragma once

#include <string>
#include <vector>

#include "../ast/ast.hpp"
#include "../shared/diagnostic.hpp"

// Tracked variable, can go out of scope
class tracked_var
{
    public:
    identifier const* name;
    var_decl const* target;
    unsigned int depth;
};

/*
Semantics tracker, is used to get the states of variables
during codegen. The tracker is also used accross files,
so that means there is one tracker for multiple codegenerators,
which means it also tracks imports.
*/
class tracker
{
    public:
    tracker(diagnostic_manager& diag)
        : diagnostics(diag) {}

    // Returns true if a variable with same name DOESN'T exist
    // Return false otherwise
    bool bind_var(identifier const* name, var_decl const* var);

    // If var doesn't exist, returns nullptr
    var_decl const* lookup_var(identifier const* name) const;

    // Get's out of the current scope
    // and check for variabless that are deeper then
    // current scope, and deletes them.
    void push_scope();

    // Get's deeper in the scope
    void pop_scope();

    // If called and no entry exists, will set the entry_set flag
    // Returns false if an entry existed before
    // Returns true if no entry existed before
    bool request_entry();

    private:
    // Depth in the scope
    unsigned int depth = 0;
    // True if entry is already set
    bool entry_set = false;

    // Keeps track of all the variables
    std::vector<tracked_var> vars;

    diagnostic_manager& diagnostics;
};
