#include "codegen.hpp"

std::string codegen::emit_decl(decl_stmt node)
{
    std::string result;
    if(!m_tracker->register_var(node.ident.namespaces,node.ident.name))
    {
        result = "// declaration error here\n";
        helper e;
        e.l = location(node.line, this->filename);
        e.msg = "A variable with the same name already exists";
        e.type = helper_type::line_err;
        this->helpers.push_back(e);
        this->errored = true;
        return result;
    }
    result += emit_datatype(node.data_type);
    result += " ";
    result += emit_ident(node.ident);
    result += ";\n";
    return result;
}

std::string codegen::emit_def(def_stmt node)
{
    std::string result;
    if(!m_tracker->check_var(node.ident.namespaces,node.ident.name))
    {
        this->errored = true;
        result += "// error here\n";
        helper e;
        e.l = location(node.line, this->filename);
        e.msg = "Cannot define an unexisting variable.";
        e.type = helper_type::line_err;
        this->helpers.push_back(e);
        return result;
    }
    result += emit_ident(node.ident);
    result += " = ";
    result += emit_expr(node.value);
    result += ";\n";
    return result;
}

std::string codegen::emit_decldef(decldef_stmt node)
{
    std::string result;
    result += emit_datatype(node.decl.data_type);
    result += " ";
    result += emit_ident(node.decl.ident);
    result += " = ";
    result += emit_expr(node.def.value);
    result += ";\n";
    return result;
}


std::string codegen::emit_ret(ret_stmt node)
{
    std::string result;
    result += "return ";
    result += emit_expr(node.val);
    result += ";\n";
    return result;
}

std::string codegen::emit_stmt(std::shared_ptr<stmt> s)
{
    std::string result;
    switch(s->type)
    {
        case stmt_type::compound:
        result += emit_compound(*static_cast<compound_stmt*>(s.get()));
        break;
        case stmt_type::decl:
        result += emit_decl(*static_cast<decl_stmt*>(s.get()));
        break;
        case stmt_type::def:
        result += emit_def(*static_cast<def_stmt*>(s.get()));
        break;
        case stmt_type::decldef:
        result += emit_decldef(*static_cast<decldef_stmt*>(s.get()));
        break;
        case stmt_type::ret:
        result += emit_ret(*static_cast<ret_stmt*>(s.get()));
        break;
    }
    return result;
}

std::string codegen::emit_compound(compound_stmt cstmt)
{
    std::string result;
    result += "{\n";
    this->m_tracker->scope_down();
    for(std::shared_ptr<stmt> s : cstmt.body)
    {
        result += emit_stmt(s);
    }
    this->m_tracker->scope_up();
    result += "}\n";
    return result;
}