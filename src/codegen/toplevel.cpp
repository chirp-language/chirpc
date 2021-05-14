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

void codegen::gen_toplevel()
{
    // The entry point will always be the last thing
    if (m_tree->entry)
        result += emit_entry_decl(*m_tree->entry);
    for (auto const& d : m_tree->fdecls)
        result += emit_func_decl(*d);
    for (auto const& d : m_tree->fdefs)
        result += emit_func_def(*d);
}
