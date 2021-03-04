#include "codegen.hpp"

std::string codegen::emit_args(arguments node)
{
    std::string result;
    result += "(";
    int i = 0;
    for(expr e : node.body)
    {
        result += emit_expr(e);
        if(i < node.body.size() - 1)
        {
            result += ",";
        }
        i++;
    }
    result += ")";
    return result;
}

std::string codegen::emit_fcall(func_call_stmt node)
{
    std::string result;
    result += emit_ident(node.ident);
    result += emit_args(node.args);
    return result;
}