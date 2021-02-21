#include "ast.hpp"

#include <iostream>
#include <queue>
#include <utility>

// === UTILS FUNCTIONS FOR THE UTIL AST DUMPS ===
// ^ meta

std::string indent(int x)
{
    std::string result;
    for (int i = 0; i < x; i++)
    {
        result += "\t";
    }
    return result;
}

// I don't even care about names now
std::string dump_dtname(dtypename n)
{
    std::string result;
    switch(n)
    {
        case dtypename::_int:
        result = "int";
        break;
        case dtypename::_float:
        result = "float";
        break;
        case dtypename::_double:
        result = "double";
        break;
        case dtypename::_char:
        result = "char";
        break;
        case dtypename::_byte:
        result = "byte";
        break;
        case dtypename::_bool:
        result = "bool";
        break;
        case dtypename::_none:
        result = "none";
        break;
    }
    return result;
}

std::string dump_dtmod(dtypemod m)
{
    std::string result;
    switch(m)
    {
        case dtypemod::_ptr:
        result = "ptr";
        break;
        case dtypemod::_signed:
        result = "signed";
        break;
        case dtypemod::_unsigned:
        result = "unsigned";
        break;
        case dtypemod::_const:
        result = "const";
        break;
        case dtypemod::_func:
        result = "func";
        break;
    }
    return result;
}

// === AST UTIL DUMPS ===
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

std::string parameters::dump(int depth)
{

}

std::string stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "default_statement:\n";
    return result;
}

std::string dtype::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "data_type:\n";
    result += indent(depth+1);
    result += "typename: ";
    result += dump_dtname(this->tname);
    result += ";\n";
    result += indent(depth+1);
    if(this->tmods.size() == 0){
        result += "(no type modifiers)\n";
    }
    else{
        result += "type modifiers:\n";
        // I really gave up on naming thing well there
        for(char x : this->tmods){
            dtypemod w = static_cast<dtypemod>(x);
            result += indent(depth+2);
            result += dump_dtmod(w);
            result += "\n";
        }
    }
    return result;
}

std::string decl_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "decl_statement;\n";
    this->type.dump(depth+1);
    return result;
}

std::string def_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "def_statement;\n";
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
    result += "(no_args)";
    result += ":\n";
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

std::string func_def_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "function_definition:\n";
    return result;
}

std::string func_decl_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "function_declaration:\n";
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