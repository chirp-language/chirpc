#include "codegen.hpp"

std::string codegen::emit_arguments(arguments const& node)
{
    std::string result;
    result += "(";
    int i = 0;
    for(auto& e : node.body)
    {
        result += emit_expr(*e);
        if(i < node.body.size() - 1)
        {
            result += ",";
        }
        i++;
    }
    result += ")";
    return result;
}

std::string codegen::emit_func_call(func_call const& node)
{
    std::string result;
    result += emit_expr(*node.callee);
    result += emit_arguments(node.args);
    return result;
}

std::string codegen::emit_parameters(parameters const& node)
{
    std::string result;
    result += '(';

    bool first = true;
    for (auto const& param : node.body)
    {
        if (first)
            first = false;
        else
            result += ", ";
        auto const& var = *param;

        result += emit_datatype(var.var_type);
        result += " ";
        result += emit_identifier(var.ident);
    }

    result += ')';
    return result;
}

std::string codegen::emit_func_decl(func_decl const& node)
{
    std::string result;
    result += emit_datatype(node.data_type);
    result += ' ';
    result += emit_identifier(node.ident);
    result += emit_parameters(node.params);
    result += ";\n";
    return result;
}

std::string codegen::emit_func_def(func_def const& node)
{
    std::string result;
    result += emit_datatype(node.data_type);
    result += ' ';
    result += emit_identifier(node.ident);
    result += emit_parameters(node.params);
    result += '\n';
    result += emit_compound_stmt(*node.body);
    return result;
}
