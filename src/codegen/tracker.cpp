#include "tracker.hpp"

bool tracker::bind_var(identifier const* name, var_decl const* var)
{
    if (lookup_var(name))
        return false;

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
            auto res = it->target;
            auto res_depth = it->depth;
            // Search the current depth only
            ++it;
            while (it != end and it->depth == res_depth)
            {
                if (it->name->name == name->name)
                    // Found a collision
                    // TODO: Report error
                    return nullptr;
                ++it;
            }
            return res;
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
