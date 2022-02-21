#pragma once

#include <memory>
#include <string>
#include <list>
#include <vector>

#include "../ast/ast.hpp"
#include "../shared/diagnostic.hpp"

// Tracked symbol, can go out of scope
class tracker_symbol
{
    public:
    //tracker_symbol* parent = nullptr;
    identifier name;
    raw_qual_identifier full_name;
    decl* target = nullptr;

    // Flags
    bool has_name : 1;
    bool is_global : 1; // Is accessible in global scope? Is exportable?
    bool has_storage : 1; // Does this symbol declare storage? Is a physical entity in the program?
    bool is_entry : 1;
    bool is_scope : 1; // Does this symbol define a scope?

    tracker_symbol()
        : has_name(false), is_global(true), has_storage(false), is_entry(false), is_scope(false)
    {}
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
    using symbol = tracker_symbol;

    // List of currently availble symbols in scope
    using symlist = std::list<symbol*>;
    using symiter = symlist::iterator;

    // Table of all symbols defined in the program
    using symtable = std::vector<std::unique_ptr<symbol>>;

    struct scope
    {
        symbol* sym;
        symiter begin;
    };

    tracker(diagnostic_manager& diag, ast_root* root)
        : top_scope(root), diagnostics(diag)
    {
        gen_top_symbol();
    }

    // Delare a symbol in current scope (without binding it)
    symbol* decl_sym();
    symbol* decl_sym(identifier const& name, decl& target);

    // Returns true if a symbol with same name in current scope DOESN'T exist
    // Return false otherwise
    bool bind_sym(symbol* sym);

    // Binds a new symbol regardless of whether it's present in the current scope
    void push_sym_unsafe(symbol* sym);

    // Searches a symbol with the same name in current scope only
    // Returns nullptr if symbol is not found
    symbol* find_sym_cur(identifier const& name);

    // If symbol doesn't exist, returns nullptr
    symbol* lookup_sym(identifier const& name);
    symbol* lookup_sym_qual(qual_identifier const& name);

    symbol* lookup_decl_sym(decl const& decl_scope, identifier const& name);

    // Gets deeper in the scope
    void push_scope(symbol* sym);

    // Gets out of the current scope
    // and check for variables that are deeper then
    // current scope, and deletes them.
    void pop_scope();

    symbol* get_top()
    {
        return table[0].get();
    }

    symbol* get_scope()
    {
        return scopes.back().sym;
    }

    private:
    // Depth in the scope
    unsigned int depth = 0;
    // True if entry is already set
    bool entry_set = false;

    // Keeps track of all the variables
    symlist syms;
    std::list<scope> scopes;
    ast_root* top_scope = nullptr;

    symtable table;

    void gen_top_symbol();
    raw_qual_identifier consolidate_path(identifier const& name);

    diagnostic_manager& diagnostics;

    friend class text_symbol_dumper;
};
