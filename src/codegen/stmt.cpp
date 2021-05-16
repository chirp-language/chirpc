#include "codegen.hpp"

std::string codegen::emit_decl(decl const& node)
{
    switch (node.type)
    {
        case decl_type::var:
            return emit_var_decl(static_cast<var_decl const&>(node));
        case decl_type::entry:
            return emit_entry_decl(static_cast<entry_decl const&>(node));
        /*case decl_type::import:
            return emit_import_decl(static_cast<import_decl const&>(node));
        case decl_type::fdecl:
            return emit_func_decl(static_cast<func_decl const&>(node));
        case decl_type::fdef:
            return emit_func_def(static_cast<func_def const&>(node));
        case decl_type::external:
            return emit_extern_decl(static_cast<extern_decl const&>(node));*/
    }
    #ifndef NDEBUG
    return "\n#error Bad declaration, this is a bug\n";
    #else
    __builtin_unreachable();
    #endif
}

std::string codegen::emit_var_decl(var_decl const& node)
{
    std::string result;

    if (!m_tracker->bind_var(node.ident.get(), &node))
    {
        result = "// declaration error here\n";
        diagnostic e;
        e.l = node.loc;
        e.msg = "A variable with the same name already exists";
        e.type = diagnostic_type::location_err;
        this->diagnostics.show(e);
        this->errored = true;
        return result;
    }

    // Check for invalid type
    if(node.var_type.basetp == dtypename::_none)
    {
        // Checks if it's a pointer.
        bool is_ptr = false;
        for(std::byte d : node.var_type.exttp)
        {
            if(static_cast<dtypemod>(d) == dtypemod::_ptr)
            {
                is_ptr = true;
                break;
            }
        }

        if(!is_ptr)
        {
            diagnostic e;
            e.msg = "Variable cannot be of type `none`, unless a pointer";
            e.type = diagnostic_type::location_err;
            e.l = node.loc;
            this->diagnostics.show(e);
            this->errored = true;
            return "/*errored here*/";
        }
    }
    
    result += emit_datatype(node.var_type);
    result += " ";
    result += emit_identifier(*node.ident);
    if (node.init)
    {
        result += " = ";
        result += emit_expr(*node.init);
    }
    result += ";\n";
    return result;
}

std::string codegen::emit_assign_stmt(assign_stmt const& node)
{
    std::string result;

    auto var = m_tracker->lookup_var(node.ident.get());
    if (!var)
    {
        this->errored = true;
        result += "// error here\n";
        diagnostic e;
        e.l = node.loc;
        e.msg = "Cannot assign to a non-existant variable";
        e.type = diagnostic_type::location_err;
        this->diagnostics.show(e);
        return result;
    }

    const_cast<assign_stmt&>(node).target = const_cast<var_decl*>(var); // Keep track of the assigned variable (move to semantic analysis)
    result += emit_identifier(*node.ident);
    result += " = ";
    result += emit_expr(*node.value);
    result += ";\n";
    return result;
}


std::string codegen::emit_ret_stmt(ret_stmt const& node)
{
    std::string result;
    result += "return ";
    result += emit_expr(*node.val);
    result += ";\n";
    return result;
}

std::string codegen::emit_stmt(stmt const& s)
{
    std::string result;
    switch (s.type)
    {
        case stmt_type::compound:
            result += emit_compound_stmt(static_cast<compound_stmt const&>(s));
            break;
        case stmt_type::expr:
            result += emit_expr(*static_cast<expr_stmt const&>(s).node);
            result += ";\n"; // Because this is kindof an expression stuff
            break;
        case stmt_type::decl:
            result += emit_decl(*static_cast<decl_stmt const&>(s).inner_decl);
            break;
        case stmt_type::assign:
            result += emit_assign_stmt(static_cast<assign_stmt const&>(s));
            break;
        case stmt_type::ret:
            result += emit_ret_stmt(static_cast<ret_stmt const&>(s));
            break;
        case stmt_type::conditional:
            result += emit_conditional_stmt(static_cast<conditional_stmt const&>(s));
            break;
        case stmt_type::iteration:
            result += emit_iteration_stmt(static_cast<iteration_stmt const&>(s));
            break;
        case stmt_type::null:
            break; // emit nothing
    }
    return result;
}

std::string codegen::emit_compound_stmt(compound_stmt const& cstmt)
{
    std::string result;
    result += "{\n";
    this->m_tracker->push_scope();
    
    for (auto& s : cstmt.body)
    {
        result += emit_stmt(*s);
    }
    
    this->m_tracker->pop_scope();
    result += "}\n";
    return result;
}

std::string codegen::emit_conditional_stmt(conditional_stmt const& node)
{
    std::string result;
    result += "if (";
    result += emit_expr(*node.cond);
    result += ")\n";
    result += emit_stmt(*node.true_branch);
    if (node.false_branch)
    {
        result += "else\n";
        result += emit_stmt(*node.false_branch);
    }
    return result;
}

std::string codegen::emit_iteration_stmt(iteration_stmt const& node)
{
    std::string result;
    result += "while (";
    result += emit_expr(*node.cond);
    result += ")\n";
    result += emit_stmt(*node.loop_body);
    return result;
}
