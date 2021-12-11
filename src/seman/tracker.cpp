#include "tracker.hpp"

#include <algorithm>

tracker::symbol* tracker::decl_sym()
{
    auto* sym = new symbol;
    table.push_back(std::unique_ptr<tracker::symbol>(sym));
    return sym;
}

tracker::symbol* tracker::decl_sym(identifier const& name, decl& target)
{
    auto* sym = decl_sym();
    sym->has_name = !name.name.empty();
    sym->name = name;
    sym->target = &target;
    return sym;
}

bool tracker::bind_sym(tracker::symbol* sym)
{
    // Search for collisions, the current depth only
    if (auto* prev = find_sym_cur(sym->name))
    {
        // Found a collision
        {
            diagnostic(diagnostic_type::location_err)
                .at(sym->target->loc)
                .reason("Redefinition of a symbol")
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
    push_sym_unsafe(sym);
    return true;
}

void tracker::push_sym_unsafe(tracker::symbol* sym)
{
    if (sym->target)
        sym->target->symbol = sym;
    if (sym->has_name)
    {
        if (sym->is_global)
            sym->full_name = consolidate_path(sym->name);
        else
            sym->full_name.parts.push_back(sym->name);
    }
    syms.push_back(sym);
    // FIXME: This only considers the immediately enclosing scope
    if (scopes.back().begin == syms.end())
        --scopes.back().begin;
}

tracker::symbol* tracker::find_sym_cur(identifier const& name)
{
    for (auto it = syms.rbegin(), end = std::reverse_iterator(scopes.back().begin); it != end; ++it)
    {
        if ((**it).name.name == name.name)
            return & **it;
    }
    return nullptr;
}

// Linear Search, inefficient.
tracker::symbol* tracker::lookup_sym(identifier const& name)
{
    auto lastend = syms.cend();
    for (auto scopeit = scopes.rbegin(), scopeend = scopes.rend(); scopeit != scopeend; ++scopeit)
    {
        if (scopeit->begin == syms.cend())
            // Skip empty scope
            continue;
        for (symlist::const_iterator it = scopeit->begin; it != lastend; ++it)
        {
            if ((**it).name.name == name.name)
            {
                return *it;
            }
        }
        lastend = scopeit->begin;
    }
    return nullptr;
}

tracker::symbol* tracker::lookup_sym_qual(qual_identifier const& name)
{
    symbol* scope;
    identifier const* id = &name.parts.at(0);
    if (name.is_global)
        scope = get_top();
    else
        scope = lookup_sym(*id);
    if (!scope)
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
        scope = lookup_decl_sym(*scope->target, *id);
        if (!scope)
            return nullptr;
    }
    return scope;
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

tracker::symbol* tracker::lookup_decl_sym(decl const& decl_scope, identifier const& name)
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
            return (**begin).symbol;
        ++begin;
    }
    diagnostic(diagnostic_type::location_err)
        .at(name.loc)
        .reason("Couldn't resolve identifier")
        .report(diagnostics);
    return nullptr;
}

void tracker::push_scope(symbol* sym)
{
    ++depth;
    auto& s = scopes.emplace_back();
    s.begin = syms.end();
    s.sym = sym;
    sym->is_scope = true;
}

// This function is basically efficient
void tracker::pop_scope()
{
    syms.erase(scopes.back().begin, syms.end());
    scopes.pop_back();
    --depth;
}

void tracker::gen_top_symbol()
{
    auto* top = decl_sym();
    top->target = top_scope;
    top->is_global = true;
    top->has_storage = false;
    push_scope(top);
}

raw_qual_identifier tracker::consolidate_path(identifier const& name)
{
    raw_qual_identifier id;
    for (auto const& scope : scopes)
    {
        if (scope.sym and scope.sym->has_name)
        {
            id.parts.push_back(scope.sym->name);
        }
    }
    id.parts.push_back(name);
    return id;
}
