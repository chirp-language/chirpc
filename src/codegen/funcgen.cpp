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
