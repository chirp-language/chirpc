#include "codegen.hpp"

#include "../ast/types.hpp"

std::string codegen::emit_ident(identifier& ident)
{
    std::string result;

    // Namespaces are complicated and need more tracking, so they aren't generated yet

    result += ident.name;

    return result;
}

// This is not finished
std::string codegen::emit_datatype(exprtype& d)
{
    // Doesn't do function pointery things
    std::string result;

    int ptr_depth = 0;
    bool is_const = false;

    for (std::byte d : d.exttp)
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

    switch (d.basetp)
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
            result += "none";
            break;
    }

    for (int i = 0; i < ptr_depth; i++)
        result += "*";

    return result;
}

std::string codegen::emit_literal(literal_node& node)
{
    std::string result;
    if (node.ltype == littype::num)
    {
        num_literal lit = static_cast<num_literal&>(node);
        result = lit.value;
    }
    else if (node.ltype == littype::txt)
    {
        txt_literal lit = static_cast<txt_literal&>(node);
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
        result = "#error litteral has undefined type\n";
    }
    return result;
}

std::string codegen::emit_op(binop& node)
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

std::string codegen::emit_expr(expr& node)
{
    switch (node.kind)
    {
        case optype::lit:
            return emit_literal(static_cast<literal_node&>(node));
        case optype::ident:
            return emit_ident(static_cast<identifier&>(node));
        case optype::call:
            return emit_fcall(static_cast<func_call&>(node));
        case optype::op:
            return emit_op(static_cast<binop&>(node));
        case optype::invalid:
        default:
            return "\n#error Bad operand, This is a bug";
    }
}
