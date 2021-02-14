#include "ast.hpp"

#include <iostream>
#include <queue>
#include <utility>

std::string indent(int x)
{
    std::string result;
    for (int i = 0; i < x; i++)
    {
        result += "\t";
    }
    return result;
}

// === UTIL DUMPS ===
// Doesn't dump in any particular format(yet)

std::string ast::dump()
{
    std::string result;
    result = root.dump(0);
    return result;
}

std::string ast_node::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "default_node:\n";
    for (auto& child : this->children)
    {
        result += child->dump(depth + 1);
    }
    return result;
}

std::string identifier::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "identifier <";
    int i = 0;
    for (std::string wot : this->namespaces)
    {
        result += wot;
        if (i != this->namespaces.size() - 1)
        {
            result += ",";
        }
    }
    result += "> ";
    result += this->name;
    result += "\n";
    return result;
}

std::string literal_node::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "default_literal";
    return result;
}

std::string txt_literal::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "text_literal ";
    if(this->single_char){
        result += "(char) ";
    }
    else{
        result += "(string) ";
    }
    result += "\"";
    result += value;
    result += "\";\n";
    return result;
}

std::string num_literal::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "number_literal ";
    result += value;
    result += ";\n";
    return result;
}


std::string mathop::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "Math Operator ";
    result += this->type;
    result += ":\n";
    result += this->left.dump(depth+1);
    result += this->right.dump(depth+1);
    return result;
}

// Operator precedence?? What is that
std::string mathexpr::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "mathexpr:\n";

    for(mathop& op : operands)
    {
        result += op.dump(depth+1);
    }

    return result;
}

std::string staticexpr::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "static_expression:\n";
    result += this->value.get()->dump(depth+1);
    return result;
}

std::string expr::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "basic_expression;\n";
    return result;
}

std::string arguments::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "arguments:\n";
    for(auto n : body)
    {
        result += n.get()->dump(depth+1);
    }
    return result;
}

std::string stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "default_statement:\n";
    return result;
}

std::string compound_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "compound_statement:\n";
    for (auto& s : this->body)
    {
        result += s->dump(depth + 1);
    }
    return result;
}

std::string entry_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "entry_statement ";
    if(this->has_args)
    {
        result += "(has_args)";
        result += ":\n";
        result += this->args->dump(depth+1);
    }
    else{
        result += "(no_args)";
        result += ":\n";
    }
    result += this->code->dump(depth + 1);
    return result;
}

std::string import_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "import_statement:\n";
    result += this->filename.dump(depth + 1);
    return result;
}

std::string ret_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "ret_statement:\n";
    result += this->val->dump(depth + 1);
    return result;
}

std::string func_call_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "function_call:\n";
    result += this->ident.dump(depth + 1);
    result += this->args.dump(depth + 1);
    return result;
}