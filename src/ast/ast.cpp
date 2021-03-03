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
        //result += "\t";
        result += "   ";
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
    result += "Top Level:\n";

    if(this->imports.size() == 0)
    {
        result += "-- No imports --\n";
    }
    else
    {
        for(import_stmt import : this->imports)
        {
            result += import.dump(0);
        }
    }

    if(this->externs.size() == 0)
    {
        result += "-- No externs --\n";
    }
    else
    {
        for(extern_stmt ext : this->externs)
        {
            result += ext.dump(1);
        }
    }

    if(this->fdecls.size() == 0)
    {
        result += "-- No function declarations on top-level --\n";
    }
    else
    {
        for(func_decl_stmt node : this->fdecls)
        {
            result += node.dump(1);
        }
    }

    if(this->fdefs.size() == 0)
    {
        result += "-- No function definitions on top-level --\n";
    }
    else
    {
        for(func_def_stmt node : this->fdefs)
        {
            result += node.dump(1);
        }
    }

    if(this->has_entry)
    {
        result += this->entry.dump(0);
    }
    else
    {
        result += "-- No entry --\n";
    }
    return result;
}

std::string ast_node::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "default_node:\n";
    //for (auto& child : this->children)
    //{
    //    result += child->dump(depth + 1);
    //}
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

std::string subexpr::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "subexpr ";
    result += op.dump(0);
    //result += ");\n";
    result += indent(depth+1);
    result += "left:\n";
    result += left.dump(depth+1);
    result += indent(depth+1);
    result += "right:\n";
    result += right.dump(depth+1);
    return result;
}

std::string exprop::dump(int depth)
{
    std::string result;
    result = indent(depth);
    result += "expr_operator (";
    switch(this->type){
        case '+':
        result += "add";
        break;
        case '-':
        result += "sub";
        break;
        case '/':
        result += "div";
        break;
        case '*':
        result += "mult";
        break;
        case 'd':
        result += "deref";
        break;
        case 'r':
        result += "ref";
        break;
        case 'a':
        result += "as";
        break;
    }
    result += ");\n";
    return result;
}

std::string operand::dump(int depth)
{
    std::string result;
    result = indent(depth);
    result += "operand:\n";
    switch(this->type)
    {
        case optype::lit:
        result += static_cast<literal_node*>(this->node.get())->dump(depth+1);
        break;
        case optype::ident:
        result += static_cast<identifier*>(this->node.get())->dump(depth+1);
        break;
        case optype::call:
        result += static_cast<func_call_stmt*>(this->node.get())->dump(depth+1);
        break;
        case optype::subexpr:
        result += static_cast<subexpr*>(this->node.get())->dump(depth+1);
        break;
        case optype::op:
        result += static_cast<exprop*>(this->node.get())->dump(depth+1);
        break;
        case optype::invalid:
        result += indent(depth+1);
        result += "(INVALID EXPR)\n";
        break;
    }
    return result;
}

std::string expr::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "expression;\n";
    result += this->root.dump(depth+1);
    return result;
}

std::string arguments::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "arguments:\n";
    for(auto n : body)
    {
        result += n.dump(depth+1);
    }
    return result;
}

std::string parameters::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "parameters:\n";
    for(decl_stmt param : this->body)
    {
    //    result += indent(depth+1);
        result += param.dump(depth+1);
    //    result += "\n";
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

std::string decldef_stmt::dump(int depth)
{
    std::string result;
    result = indent(depth);
    result += "declaration&definition:\n";
    result += this->decl.dump(depth+1);
    result += this->def.dump(depth+1);
    return result;
}

std::string decl_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "decl_statement:\n";
    result += this->data_type.dump(depth+1);
    result += this->ident.dump(depth+1);
    return result;
}

std::string def_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "def_statement:\n";
    result += this->ident.dump(depth+1);
    result += this->value.dump(depth+1);
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
    result += this->val.dump(depth + 1);
    return result;
}

std::string extern_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "extern (";
    switch(this->type)
    {
        case 0:
        result += "None";
        break;
        case 1:
        result += "Function";
        break;
        case 2:
        result += "Variable";
        break;
    }
    result += "): \n";

    if(this->type == 1)
    {
        result += static_cast<func_decl_stmt*>(this->stmt.get())->dump(depth+1);
    }
    else if(this->type == 2)
    {
        result += static_cast<decl_stmt*>(this->stmt.get())->dump(depth+1);
    }

    return result;
}

std::string func_def_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "function_definition:\n";
    result += data_type.dump(depth+1);
    result += ident.dump(depth+1);
    result += params.dump(depth+1);
    result += body.dump(depth+1);
    return result;
}

std::string func_decl_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "function_declaration:\n";
    result += data_type.dump(depth+1);
    result += ident.dump(depth+1);
    result += params.dump(depth+1);
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