#include "codegen.hpp"

#include "../ast/types.hpp"

std::string codegen::emit_ident(identifier ident)
{
    std::string result;

    // Namespaces are complicated and need more tracking, so they aren't generated yet

    result += ident.name;

    return result;
}

// This is not finished
std::string codegen::emit_datatype(dtype d)
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

std::string codegen::emit_literal(std::shared_ptr<literal_node> node)
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

std::string codegen::emit_subexpr(subexpr node)
{
    std::string result;

    result += "(";
    result += emit_operand(node.left);
    result += ") ";

    if(node.op.type == 'd')
    {
        result += "\n#derefs don't work yet\n";
    }
    else if(node.op.type == 'r')
    {
        result += "\n#refs don't work yet\n";
    }
    else if(node.op.type == 'a')
    {
        result += "\n#error casts don't work yet\n";
    }
    else
    {
        result += node.op.type;
    }

    result += " (";
    result += emit_operand(node.right);
    result += ")";

    return result;
}

std::string codegen::emit_operand(operand node)
{
    if(node.type == optype::lit)
    {
        if(static_cast<literal_node*>(node.node.get())->ltype == littype::txt)
        {
            return emit_literal(std::make_shared<txt_literal>(
                *static_cast<txt_literal*>(node.node.get())
                ));
        }
        else if(static_cast<literal_node*>(node.node.get())->ltype == littype::num)
        {
            return emit_literal(std::make_shared<num_literal>(
                *static_cast<num_literal*>(node.node.get())
                ));
        }
        else
        {
            // just panic idk
            this->errored = true;
        }
    }
    else if(node.type == optype::ident)
    {
        return emit_ident( *static_cast<identifier*>(node.node.get()) );
    }
    else if(node.type == optype::call)
    {
        return emit_fcall( *static_cast<func_call_stmt*>(node.node.get()) );
    }
    else if(node.type == optype::subexpr)
    {
        return emit_subexpr(*static_cast<subexpr*>(node.node.get()));
    }
    else if(node.type == optype::op || node.type == optype::invalid)
    {
        // error
        this->errored = true;
        return "\n#error Bad operand, This error message is temporary";
    }

    return "\n#error what\n";
}

std::string codegen::emit_expr(expr node)
{
    return emit_operand(node.root);
}