#include "codegen.hpp"

std::string codegen::emit_entry_decl(entry_decl const& e)
{
    std::string result;
    if (m_tracker->request_entry())
    {
        result += "// Reminder this doesn't check for an existing int main().\n";
        result += "int main()";
        result += emit_compound_stmt(static_cast<compound_stmt const&>(*e.code));
    }
    else
    {
        result += "// Error here\n";
    }
    return result;
}

std::string codegen::emit_namespace(namespace_decl const& e)
{
    // This is hacky beyond belief
    std::string result;

    for(auto const& d : e.fdecls)
    {
        d.get()->ident.get()->name = e.ident.get()->name + '_' + d.get()->ident.get()->name;
        result += emit_func_decl(*d);
    }

    for(auto const& d : e.fdefs)
    {
        d.get()->ident.get()->name = e.ident.get()->name + '_' + d.get()->ident.get()->name;
        result += emit_func_def(*d);
    }

    return result;
}

void codegen::gen_toplevel()
{
    for(auto const& d: m_tree->nspaces)
        result += emit_namespace(*d);
    for (auto const& d : m_tree->fdecls)
        result += emit_func_decl(*d);
    for (auto const& d : m_tree->fdefs)
        result += emit_func_def(*d);
    // The entry point will always be the last thing
    if (m_tree->entry)
        result += emit_entry_decl(*m_tree->entry);
}
