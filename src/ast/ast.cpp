#include "ast.hpp"

#include <iostream>
#include <queue>
#include <utility>

// === UTILS FUNCTIONS FOR THE UTIL AST DUMPS ===
// ^ meta

std::string indent(int x)
{
    std::string result;
    result.resize(x * 3, ' ');
    return result;
}

// I don't even care about names now
std::string dump_dtname(dtypename n)
{
    std::string result;
    switch (n)
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
    switch (m)
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

std::string ast_root::dump()
{
    std::string result;
    result += "Top Level:\n";

    if (this->imports.size() == 0)
    {
        result += "-- No imports --\n";
    }
    else
    {
        for (auto& import : this->imports)
        {
            result += import->dump(0);
        }
    }

    if (this->externs.size() == 0)
    {
        result += "-- No externs --\n";
    }
    else
    {
        for (auto& ext : this->externs)
        {
            result += ext->dump(1);
        }
    }

    if (this->fdecls.empty())
    {
        result += "-- No function declarations on top-level --\n";
    }
    else
    {
        for (auto& node : this->fdecls)
        {
            result += node->dump(1);
        }
    }

    if (this->fdefs.empty())
    {
        result += "-- No function definitions on top-level --\n";
    }
    else
    {
        for (auto& node : this->fdefs)
        {
            result += node->dump(1);
        }
    }

    if (this->has_entry)
    {
        result += this->entry.dump(0);
    }
    else
    {
        result += "-- No entry --\n";
    }
    return result;
}

/*std::string ast_node::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "default_node:\n";
    //for (auto& child : this->children)
    //{
    //    result += child->dump(depth + 1);
    //}
    return result;
}*/

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
            result += ".";
        }
    }
    result += "> ";
    result += this->name;
    result += "\n";
    return result;
}

std::string loperand::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "left_operand:\n";
    return result;
}

std::string literal_node::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "none_literal";
    return result;
}

std::string txt_literal::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "text_literal ";
    if (this->is_character)
    {
        result += "(char) ";
    }
    else
    {
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

std::string binop::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "binop ";
    result += exprop_id(op);
    //result += ");\n";
    result += indent(depth + 1);
    result += "left:\n";
    result += left->dump(depth + 1);
    result += indent(depth + 1);
    result += "right:\n";
    result += right->dump(depth + 1);
    return result;
}

std::string exprop_id(exprop op)
{
    std::string result;
    result = "(";
    switch (op)
    {
    case static_cast<exprop>('+'):
        result += "add";
        break;
    case static_cast<exprop>('-'):
        result += "sub";
        break;
    case static_cast<exprop>('/'):
        result += "div";
        break;
    case static_cast<exprop>('*'):
        result += "mult";
        break;
    case exprop::deref:
        result += "deref";
        break;
    case exprop::ref:
        result += "ref";
        break;
    case exprop::as:
        result += "as";
        break;
    case exprop::call:
        result += "call";
        break;
    case exprop::none:
    default:
        result += "invalid";
    }
    result += ");\n";
    return result;
}

std::string arguments::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "arguments:\n";
    for (auto& n : body)
    {
        result += n->dump(depth + 1);
    }
    return result;
}

std::string parameters::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "parameters:\n";
    for (auto& param : this->body)
    {
        //    result += indent(depth+1);
        result += param->dump(depth + 1);
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

std::string exprtype::dump(int depth) const
{
    std::string result;
    result += indent(depth);
    result += "data_type:\n";
    result += indent(depth + 1);
    result += "typename: ";
    result += dump_dtname(basetp);
    result += ";\n";
    result += indent(depth + 1);
    if (exttp.empty())
    {
        result += "(no type modifiers)\n";
    }
    else
    {
        result += "type modifiers:\n";
        // I really gave up on naming thing well there
        for (std::byte x : exttp)
        {
            dtypemod w = static_cast<dtypemod>(x);
            result += indent(depth + 2);
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
    result += "decl_statement:\n";
    result += this->data_type.dump(depth + 1);
    result += this->ident->dump(depth + 1);
    if (this->init)
        result += this->init->dump(depth + 1);
    return result;
}

std::string def_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "def_statement:\n";
    result += this->ident->dump(depth + 1);
    result += this->value->dump(depth + 1);
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

std::string extern_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "extern (";
    
    switch (this->type)
    {
        case extern_stmt::decl_type::None:
            result += "None";
            break;
        case extern_stmt::decl_type::Function:
            result += "Function";
            break;
        case extern_stmt::decl_type::Variable:
            result += "Variable";
            break;
    }
    
    result += "): \n";

    if (this->type == extern_stmt::decl_type::Function)
    {
        result += static_cast<func_decl_stmt*>(this->decl.get())->dump(depth + 1);
    }
    else if (this->type == extern_stmt::decl_type::Variable)
    {
        result += static_cast<decl_stmt*>(this->decl.get())->dump(depth + 1);
    }

    return result;
}

std::string func_def_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "function_definition:\n";
    result += data_type.dump(depth + 1);
    result += ident->dump(depth + 1);
    result += params.dump(depth + 1);
    result += body->dump(depth + 1);
    return result;
}

std::string func_decl_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "function_declaration:\n";
    result += data_type.dump(depth + 1);
    result += ident->dump(depth + 1);
    result += params.dump(depth + 1);
    return result;
}

std::string func_call::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "function_call:\n";
    result += this->callee->dump(depth + 1);
    result += this->args.dump(depth + 1);
    return result;
}

std::string expr_stmt::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "expr_stmt:\n";
    result += this->node->dump(depth + 1);
    return result;
}
