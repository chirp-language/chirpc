#pragma once

#include <string>
#include <list>

#include "../ast/ast.hpp"
#include "../shared/diagnostic.hpp"

/*
Semantics tracker, is used to get the states of variables
during codegen. The tracker is also used accross files,
so that means there is one tracker for multiple codegenerators,
which means it also tracks imports.
*/
class tracker
{
    public:

    // Tracked symbol, can go out of scope
    class tracked_sym
    {
        public:
        identifier name;
        decl const* target;
        unsigned int depth;
    };

    using symlist = std::list<tracked_sym>;
    using symiter = symlist::iterator;

    struct tracked_scope
    {
        symiter begin;
        identifier const* scope_name;
        decl const* scope_target;
    };

    tracker(diagnostic_manager& diag)
        : diagnostics(diag) {}

    void set_root(ast_root const* root) {
        top_scope = root;
    }

    // Returns true if a symbol with same name in current scope DOESN'T exist
    // Return false otherwise
    bool bind_sym(identifier const& name, decl const& target);

    // Binds a new symbol regardless of whether it's present in the current scope
    void push_sym_unsafe(identifier const& name, decl const& target);

    // Searches a symbol with the same name in current scope only
    // Returns nullptr if symbol is not found
    tracked_sym* find_sym_cur(identifier const& name);

    // If symbol doesn't exist, returns nullptr
    decl const* lookup_sym(identifier const& name) const;
    decl const* lookup_sym_qual(qual_identifier const& name) const;

    decl const* lookup_decl_sym(decl const& decl_scope, identifier const& name) const;

    // Gets deeper in the scope
    void push_scope(identifier const* name = nullptr, decl const* owner = nullptr);

    // Gets out of the current scope
    // and check for variabless that are deeper then
    // current scope, and deletes them.
    void pop_scope();

    private:
    // Depth in the scope
    unsigned int depth = 0;
    // True if entry is already set
    bool entry_set = false;

    // Keeps track of all the variables
    symlist syms;
    std::list<tracked_scope> scopes;
    ast_root const* top_scope = nullptr;

    diagnostic_manager& diagnostics;
};
