#include "codegen.hpp"

std::string codegen::emit_entry_decl(entry_decl const& e)
{
    std::string result;
    result += "// Reminder this doesn't check for an existing int main().\n";
    result += "int main()";
    result += emit_compound_stmt(static_cast<compound_stmt const&>(*e.code));
    return result;
}

std::string codegen::emit_namespace_decl(namespace_decl const& e)
{
    std::string result;

    for(auto const& d : e.decls)
    {
        result += emit_decl(*d);
    }

    return result;
}

void codegen::gen_toplevel()
{
    for (auto const& d : m_tree->top_decls)
        result += emit_decl(*d);
}
