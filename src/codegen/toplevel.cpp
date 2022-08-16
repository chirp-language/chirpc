#include "codegen.hpp"

#include <utility>

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

    // for caching
    std::vector<std::pair<std::string,std::string>> idk;

    for(auto const& d : e.decls)
    {
        std::string r;
        r = emit_decl(*d);
        idk.push_back(
            std::make_pair(
                emit_decl_symbol_name(d.get()),
                r
            ));
        result += r;
    }

    this->m_cache->keep_namespace(e,idk);

    return result;
}

void codegen::gen_toplevel()
{
    for (auto const& d : m_tree->top_decls)
        result += emit_decl(*d);
}
