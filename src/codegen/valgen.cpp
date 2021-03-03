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
std::string codegen::emit_datatype(dtype& d)
{
    // Doesn't do function pointery things
    std::string result;

    int ptr_depth = 0;
    bool is_const = false;

    for(char d : d.tmods)
    {
        dtypemod mod = static_cast<dtypemod>(d);
        if(mod == dtypemod::_ptr){
            ptr_depth++;
        }
        else if(mod == dtypemod::_const){
            is_const = true;
        }
    }

    if(is_const)
    {
        result += "const ";
    }

    switch(d.tname)
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

    for(int i = 0; i < ptr_depth; i++)
    {
        result += "*";
    }

    return result;
}

std::string codegen::emit_literal(std::shared_ptr<literal_node>& node)
{
    std::string result;
    if(node.get()->ltype == littype::num)
    {
        num_literal lit = *static_cast<num_literal*>(node.get());
        result = lit.value;
    }
    else if(node.get()->ltype == littype::txt)
    {
        txt_literal lit = *static_cast<txt_literal*>(node.get());
        if(lit.single_char)
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

std::string codegen::emit_expr(expr node)
{
    return "/*An expr should be here*/";
}