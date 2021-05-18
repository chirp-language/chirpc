#include "tracker.hpp"

bool tracker::bind_sym(identifier const* name, decl const* target)
{
    // Search for collisions, the current depth only
    if (auto prev = find_sym_cur(name))
    {
        // Found a collision
        {
            diagnostic d;
            d.type = diagnostic_type::location_err;
            d.l = target->loc;
            d.msg = "Redefinition of a variable/function";
            diagnostics.show(d);
        }
        {
            diagnostic d;
            d.type = diagnostic_type::location_note;
            d.l = prev->target->loc;
            d.msg = "Previous declaration here";
            diagnostics.show(d);
        }
        return false;
    }
    push_sym_unsafe(name, target);
    return true;
}

void tracker::push_sym_unsafe(identifier const* name, decl const* target)
{
    tracked_sym v;
    v.name = name;
    v.depth = depth;
    v.target = target;
    syms.push_back(v);
}

tracked_sym* tracker::find_sym_cur(identifier const* name)
{
    for (auto it = syms.rbegin(), end = syms.rend(); it != end; ++it)
    {
        if (it->depth != depth)
            break;

        if (it->name->name == name->name)
            return &*it;
    }
    return nullptr;
}

// Linear Search, inefficient.
decl const* tracker::lookup_sym(identifier const* name) const
{
    for (auto it = syms.rbegin(), end = syms.rend(); it != end; ++it)
    {
        if (it->name->name == name->name)
        {
            return it->target;
        }
    }
    return nullptr;
}

// This function is more efficient
void tracker::pop_scope()
{
    auto it = syms.rbegin();
    for (auto end = syms.rend(); it != end; ++it)
    {
        if (it->depth != depth)
            break;
    }
    --depth;
    syms.erase(it.base(), syms.end());
}

void tracker::push_scope()
{
    ++depth;
}
