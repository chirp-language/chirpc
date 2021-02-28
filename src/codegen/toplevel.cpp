#include "codegen.hpp"

std::string codegen::emit_entry(entry_stmt& e)
{
    std::string result;
    if(m_tracker->request_entry())
    {
        result += "// Reminder this doesn't check for an existing int main().\n";
        result += "int main()";
        result += emit_compound(*static_cast<compound_stmt*>(e.code.get()));
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
    result += emit_entry(this->m_tree.entry);
}