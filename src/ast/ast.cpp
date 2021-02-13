#include "ast.hpp"

#include <iostream>
#include <queue>
#include <utility>

std::string indent(int x){
    std::string result;
    for(int i=0;i<x;i++){
        result += "\t";
    }
    return result;
}

// === UTIL DUMPS ===
// Doesn't dump in any particular format(yet)

std::string ast::dump(){
    std::string result;
    result = root.dump(0);
    return result;
}
std::string ast_node::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "default_node:\n";
    for(ast_node* child : this->children){
        result += child->dump(depth+1);
    }
    return result;
}
std::string identifier::dump(int depth)
{
    std::string result;
    result += indent(depth);
    result += "identifier <";
    int i = 0;
    for(std::string wot : this->namespaces){
        result += wot;
        if(i != this->namespaces.size()-1){
            result+=",";
        }
    }
    result += "> ";
    result += this->name;
    return result;
}
std::string literal_node::dump(int depth){
    std::string result;
    result += indent(depth);
    result += "default_literal";
    return result;
}
std::string txt_literal::dump(int depth){
    std::string result;
    result += indent(depth);
    result += "text_literal \"";
    result += value;
    result += "\":\n";
    return result;
}
std::string num_literal::dump(int depth){
    std::string result;
    result += indent(depth);
    result += "number_literal ";
    result += value;
    result += ":\n";
    return result;
}
std::string arguments::dump(int depth){
    std::string result;
    result = "arguments";
    return result;
}
std::string stmt::dump(int depth){
    std::string result;
    result += indent(depth);
    result += "default_statement:\n";
    return result;
}
std::string compound_stmt::dump(int depth){
    std::string result;
    result += indent(depth);
    result += "compound_statement:\n";
    for(stmt* s:this->body){
        result += s->dump(depth+1);
    }
    return result;
}
std::string entry_stmt::dump(int depth){
    std::string result;
    result += indent(depth);
    result += "entry_statement:\n";
    //printf("%p\n",this->code);
    result += this->code->dump(depth);
    return result;
}
std::string import_stmt::dump(int depth){
    std::string result;
    result += indent(depth);
    result += "import_statement:\n";
    return result;
}
std::string ret_stmt::dump(int depth){
    std::string result;
    result += indent(depth);
    result += "ret_statement:\n";
    return result;
}
std::string func_call_stmt::dump(int depth){
    std::string result;
    result += indent(depth);
    result += "function_call:\n";
    return result;
}