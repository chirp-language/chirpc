#include "codegen.hpp"

#include "../ast/types.hpp"
#include <string>

std::string codegen::emit_raw_qual_identifier(raw_qual_identifier const& ident)
{
    // Call this only on fully expanded identifiers
    #ifndef NDEBUG
    if (ident.parts.empty())
    {
        return "/* OH NO: Identifier not expanded */";
    }
    #endif

    std::string result;
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

std::string codegen::emit_decl_symbol_name(decl const* node)
{
    if (node and node->symbol)
        return emit_raw_qual_identifier(node->symbol->full_name);
    return "/*! spotted unexpanded identifier */";
}

std::string codegen::emit_id_ref_expr(id_ref_expr const& node)
{
    return emit_decl_symbol_name(node.target);
}

// This is not finished
std::string codegen::emit_datatype(basic_type const& type)
{
    // Doesn't do function pointery things
    std::string result;

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
            result += "char";
            break;
        case dtypename::_bool:
            result += "_Bool";
            break;
        case dtypename::_none:
            result += "void";
            break;
    }

    for (auto d : type.exttp)
    {
        switch (static_cast<dtypemod>(d))
        {
            case dtypemod::_ptr:
                result += "*";
                break;
            case dtypemod::_signed:
                result += " signed";
                break;
            case dtypemod::_unsigned:
                result += " unsigned";
                break;
            case dtypemod::_const:
                result += " const";
                break;
            case dtypemod::_func:
                result += "/* function type not supported */";
                diagnostic(diagnostic_type::global_err)
                    .reason("Function types are not supported in codegen")
                    .report(diagnostics);
                break;
        }
    }

    return result;
}

std::string codegen::emit_string_literal(string_literal const& node)
{
    return "\"" + /*escape*/node.value + "\"";
}

std::string codegen::emit_integral_literal(integral_literal const& node)
{
    // Negative values shouldn't cause much problems, so I'm not adding parens
    return std::to_string(node.value.val);
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

std::string codegen::emit_unop(unop const& node)
{
    std::string result;

    switch (node.op)
    {
        case tkn_type::ref_op:
            result += "&";
            break;
        case tkn_type::deref_op:
            result += "*";
            break;
        case tkn_type::kw_alloca:
            // It just works, source: trust me
            result += "__builtin_alloca";
            break;
        default:
            result += exprop_id(node.op);
    }

    result += "(";
    result += emit_expr(*node.operand);
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
        case expr_kind::unop:
            return emit_unop(static_cast<unop const&>(node));
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
