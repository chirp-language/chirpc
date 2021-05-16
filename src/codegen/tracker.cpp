#include "tracker.hpp"

bool tracker::bind_var(identifier const* name, var_decl const* var)
{
    // Search for collisions, the current depth only
    for (auto it = vars.rbegin(), end = vars.rend(); it != end; ++it)
    {
        if (it->depth != depth)
            break;

        if (it->name->name == name->name)
        {
            // Found a collision
            {
                diagnostic d;
                d.type = diagnostic_type::location_err;
                d.l = var->loc;
                d.msg = "Redefinition of a variable";
                diagnostics.show(d);
            }
            {
                diagnostic d;
                d.type = diagnostic_type::location_note;
                d.l = it->target->loc;
                d.msg = "Previous declaration here";
            }
            return false;
        }
    }

    tracked_var v;
    v.name = name;
    v.depth = depth;
    v.target = var;
    vars.push_back(v);
    return true;
}

// Linear Search, inefficient.
var_decl const* tracker::lookup_var(identifier const* name) const
{
    for (auto it = vars.rbegin(), end = vars.rend(); it != end; ++it)
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
    auto it = vars.rbegin();
    for (auto end = vars.rend(); it != end; ++it)
    {
        if (it->depth != depth)
            break;
    }
    --depth;
    vars.erase(it.base(), vars.end());
}

void tracker::push_scope()
{
    ++depth;
}

bool tracker::request_entry()
{
    bool r = !entry_set;
    entry_set = true;
    return r;
}
