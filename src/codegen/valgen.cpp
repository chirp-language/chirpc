#include "codegen.hpp"

#include "../ast/types.hpp"
#include <string>

std::string codegen::emit_qual_identifier(qual_identifier const& ident)
{
    std::string result;

    // This is even more hacky beyond any belief (pt. 2)
    // Should probably (definitely) normalize access path first
    for (auto id = ident.parts.cbegin(), end = ident.parts.cend() - 1; id != end; ++id)
    {
        result += emit_identifier(*id) + "$";
    }
    result += ident.parts.back().name;
    return result;
}

std::string codegen::emit_identifier(identifier const& ident)
{
    return ident.name;
}

std::string codegen::emit_id_ref_expr(id_ref_expr const& node)
{
    return emit_qual_identifier(node.ident);
}

// This is not finished
std::string codegen::emit_datatype(basic_type const& type)
{
    // Doesn't do function pointery things
    std::string result;

    int ptr_depth = 0;
    bool is_signed = false;
    bool is_unsigned = false;
    bool is_const = false;

    for (std::byte d : type.exttp)
    {
        dtypemod mod = static_cast<dtypemod>(d);
        if (mod == dtypemod::_ptr)
        {
            ptr_depth++;
        }
        else if(mod == dtypemod::_signed)
        {
            is_signed = true;
        }
        else if(mod == dtypemod::_unsigned)
        {
            is_unsigned = true;
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
    if (is_signed)
    {
        result += "signed ";
    }
    if (is_unsigned)
    {
        result += "unsigned ";
    }

    switch (type.basetp)
    {
        case dtypename::_int:
            result += "int";
            break;
        case dtypename::_long:
            result += "long";
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
        result += '*';

    return result;
}

std::string codegen::emit_string_literal(string_literal const& node)
{
    return "\"" + /*escape*/node.value + "\"";
}

std::string codegen::emit_integral_literal(integral_literal const& node)
{
    return "(" + std::to_string(node.value.val) + ")";
}

std::string codegen::emit_nullptr_literal(nullptr_literal const& node)
{
    return "((void*)0)";
}

std::string codegen::emit_binop(binop const& node)
{
    std::string result;

    result += "(";
    result += emit_expr(*node.left);
    result += ") ";

    result += exprop_id(node.op);

    result += " (";
    result += emit_expr(*node.right);
    result += ")";

    return result;
}

std::string codegen::emit_cast_expr(cast_expr const& node)
{
    std::string result;
    result += "(";
    result += emit_datatype(node.type);
    result += ") (";
    result += emit_expr(*node.operand);
    result += ")";
    return result;
}

std::string codegen::emit_expr(expr const& node)
{
    switch (node.kind)
    {
        case expr_kind::binop:
            return emit_binop(static_cast<binop const&>(node));
        case expr_kind::call:
            return emit_func_call(static_cast<func_call const&>(node));
        case expr_kind::ident:
            return emit_id_ref_expr(static_cast<id_ref_expr const&>(node));
        case expr_kind::strlit:
            return emit_string_literal(static_cast<string_literal const&>(node));
        case expr_kind::intlit:
            return emit_integral_literal(static_cast<integral_literal const&>(node));
        case expr_kind::nulllit:
            return emit_nullptr_literal(static_cast<nullptr_literal const&>(node));
        case expr_kind::cast:
            return emit_cast_expr(static_cast<cast_expr const&>(node));
    }
    #ifndef NDEBUG
    return "\n#error Bad expression, this is a bug\n";
    #else
    __builtin_unreachable();
    #endif
}
