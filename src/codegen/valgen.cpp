#include "codegen.hpp"

#include "../ast/types.hpp"

std::string codegen::emit_identifier(identifier const& ident)
{
    std::string result;

    // Namespaces are complicated and need more tracking, so they aren't generated yet

    result += ident.name;

    return result;
}

std::string codegen::emit_id_ref_expr(id_ref_expr const& node)
{
    if (!ignore_unresolved_refs && !m_tracker->lookup_var(&node.ident))
    {
        diagnostic d;
        d.type = diagnostic_type::location_warning;
        d.l = node.loc;
        d.msg = "Referenced an undefined variable or a global function/variable (TODO)";
        diagnostics.show(d);
    }
    return emit_identifier(node.ident);
}

// This is not finished
std::string codegen::emit_datatype(exprtype const& t)
{
    // Doesn't do function pointery things
    std::string result;

    int ptr_depth = 0;
    bool is_const = false;

    for (std::byte d : t.exttp)
    {
        dtypemod mod = static_cast<dtypemod>(d);
        if (mod == dtypemod::_ptr)
        {
            ptr_depth++;
        }
        else if (mod == dtypemod::_const)
        {
            // FIX THIS !!
            is_const = true;
        }
    }

    if (is_const)
    {
        result += "const ";
    }

    switch (t.basetp)
    {
        case dtypename::_int:
            result += "int";
            break;
        case dtypename::_float:
            result += "float";
            break;
        case dtypename::_double:
            result += "double";
            break;
        case dtypename::_char:
            result += "char";
            break;
        case dtypename::_byte:
            result += "byte";
            break;
        case dtypename::_bool:
            result += "bool";
            break;
        case dtypename::_none:
            result += "void";
            break;
    }

    for (int i = 0; i < ptr_depth; i++)
        result += "*";

    return result;
}

std::string codegen::emit_literal(literal_node const& node)
{
    std::string result;
    if (node.ltype == littype::num)
    {
        num_literal lit = static_cast<num_literal const&>(node);
        result = lit.value;
    }
    else if (node.ltype == littype::txt)
    {
        txt_literal lit = static_cast<txt_literal const&>(node);
        if (lit.is_character)
        {
            result += '\'';
            result += lit.value;
            result += '\'';
        }
        else
        {
            result += '"';
            result += lit.value;
            result += '"';
        }
    }
    else
    {
        result = "\n#error litteral has undefined type\n";
    }
    return result;
}

std::string codegen::emit_binop(binop const& node)
{
    std::string result;

    result += "(";
    result += emit_expr(*node.left);
    result += ") ";

    if (node.op == exprop::none)
    {
        result += "\n#error invalid operator\n";
    }
    if (static_cast<short>(node.op) < 0)
    {
        result += "\n#error special operators don't work yet\n";
    }
    else
    {
        result += static_cast<unsigned char>(node.op);
    }

    result += " (";
    result += emit_expr(*node.right);
    result += ")";

    return result;
}

std::string codegen::emit_expr(expr const& node)
{
    switch (node.kind)
    {
        case optype::lit:
            return emit_literal(static_cast<literal_node const&>(node));
        case optype::ident:
            return emit_id_ref_expr(static_cast<id_ref_expr const&>(node));
        case optype::call:
            return emit_func_call(static_cast<func_call const&>(node));
        case optype::op:
            return emit_binop(static_cast<binop const&>(node));
        case optype::invalid:
        default:
            return "\n#error Bad operand, This is a bug\n";
    }
}
