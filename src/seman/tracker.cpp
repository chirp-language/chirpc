#include "tracker.hpp"

bool tracker::bind_sym(identifier const& name, decl const& target)
{
    // Search for collisions, the current depth only
    if (auto prev = find_sym_cur(name))
    {
        // Found a collision
        {
            diagnostic(diagnostic_type::location_err)
                .at(target.loc)
                .reason("Redefinition of a variable/function")
                .report(diagnostics);
        }
        {
            diagnostic(diagnostic_type::location_note)
                .at(prev->target->loc)
                .reason("Previous declaration here")
                .report(diagnostics);
        }
        return false;
    }
    push_sym_unsafe(name, target);
    return true;
}

void tracker::push_sym_unsafe(identifier const& name, decl const& target)
{
    tracked_sym v;
    v.name = name;
    v.depth = depth;
    v.target = &target;
    syms.push_back(v);
    if (scopes.back().begin == syms.end())
        --scopes.back().begin;
}

tracker::tracked_sym* tracker::find_sym_cur(identifier const& name)
{
    for (auto it = syms.rbegin(), end = syms.rend(); it != end; ++it)
    {
        if (it->depth != depth)
            break;

        if (it->name.name == name.name)
            return &*it;
    }
    return nullptr;
}

// Linear Search, inefficient.
decl const* tracker::lookup_sym(identifier const& name) const
{
    auto lastend = syms.cend();
    for (auto scopeit = scopes.rbegin(), scopeend = scopes.rend(); scopeit != scopeend; ++scopeit)
    {
        for (symlist::const_iterator it = scopeit->begin; it != lastend; ++it)
        {
            if (it->name.name == name.name)
            {
                return it->target;
            }
        }
        lastend = scopeit->begin;
    }
    return nullptr;
}

decl const* tracker::lookup_sym_qual(qual_identifier const& name) const
{
    decl const* top_decl;
    identifier const* id = &name.parts.at(0);
    if (name.is_global)
        top_decl = top_scope;
    else
        top_decl = lookup_sym(*id);
    if (!top_decl)
    {
        diagnostic(diagnostic_type::location_err)
            .at(id->loc)
            .reason("Couldn't resolve identifier")
            .report(diagnostics);
        return nullptr;
    }
    for (size_t ii = 1, size = name.parts.size(); ii < size; ++ii)
    {
        id = &name.parts[ii];
        top_decl = lookup_decl_sym(*top_decl, *id);
        if (!top_decl)
            return nullptr;
    }
    return top_decl;
}

static identifier const* get_declaration_name(decl const& dec)
{
    switch (dec.kind) {
        case decl_kind::var:
            return &static_cast<var_decl const&>(dec).ident;
        case decl_kind::nspace:
            return &static_cast<namespace_decl const&>(dec).ident;
        case decl_kind::fdecl:
            return &static_cast<func_decl const&>(dec).ident;
        case decl_kind::fdef:
            return &static_cast<func_def const&>(dec).ident;
        case decl_kind::external:
            return get_declaration_name(*static_cast<extern_decl const&>(dec).inner_decl);
        default:
            return nullptr;
    }
}

decl const* tracker::lookup_decl_sym(decl const& decl_scope, identifier const& name) const
{
    declh const* begin;
    declh const* end;
    switch (decl_scope.kind)
    {
        case decl_kind::root:
            begin = &static_cast<ast_root const&>(decl_scope).top_decls.front();
            end = &static_cast<ast_root const&>(decl_scope).top_decls.back() + 1;
            break;
        case decl_kind::nspace:
            begin = &static_cast<namespace_decl const&>(decl_scope).decls.front();
            end = &static_cast<namespace_decl const&>(decl_scope).decls.back() + 1;
            break;
        default:
            diagnostic(diagnostic_type::location_err)
                .at(name.loc)
                .reason("Expected a valid scope")
                .report(diagnostics);
            return nullptr;
    }
    while (begin != end)
    {
        if (auto n = get_declaration_name(**begin); name.name == n->name)
            return (*begin).get();
        ++begin;
    }
    diagnostic(diagnostic_type::location_err)
        .at(name.loc)
        .reason("Couldn't resolve identifier")
        .report(diagnostics);
    return nullptr;
}

// This function is basically efficient
void tracker::pop_scope()
{
    syms.erase(scopes.back().begin, syms.end());
    scopes.pop_back();
    --depth;
}

void tracker::push_scope(identifier const* name, decl const* target)
{
    ++depth;
    auto& s = scopes.emplace_back();
    s.begin = syms.end();
    s.scope_name = name;
    s.scope_target = target;
}
