/*
Abstract Syntax Tree definition
This contains all the possible nodes, cuz why not
The code for creating the ast, in parser
*/

/*
All possible AST nodes are defined here
*/
#include <string>
#include <vector>

// === SHARED ===

class ast_node
{
    public:
    virtual std::string dump(int);
    std::vector<ast_node*> children;
};

class ast
{
    public:
    // Could probably store more stuff than just the root node
    ast_node root;
    std::string dump();
};

// === LITERALS & SIMILAR??! ===

class identifier : public ast_node
{
    public:
    // The namespace vector for a.b.c.foo() would be like:
    // {"a","b","c"}.. Further in vector => More nested
    std::vector<std::string> namespaces;
    std::string name;
    virtual std::string dump(int);
};

class literal_node : public ast_node
{
    public:
    virtual std::string dump(int);
};

class txt_literal : public literal_node
{
    public:
    virtual std::string dump(int);
    bool single_char;
    std::string value;
};

// Should be replaced by a different literal for each types
class num_literal : public literal_node
{
    public:
    std::string value;
    virtual std::string dump(int);
};

// === Stuff that likes has literals in it but not a statement ish

class arguments : public ast_node
{
    public:
    // expression?? lol what's that 
    std::vector<literal_node*> body;
    virtual std::string dump(int);
};

// === STATEMENTS ===

class stmt : public ast_node
{
    public:
    // It's kindof a location-ish
    int line;
    virtual std::string dump(int);
};

class compound_stmt : public stmt
{
    public:
    std::vector<stmt*> body;
    virtual std::string dump(int);
};

class entry_stmt : public stmt
{
    public:
    stmt* code;
    virtual std::string dump(int);
};

class import_stmt : public stmt
{
    public:
    txt_literal filename;
    virtual std::string dump(int);
};

class ret_stmt : public stmt
{
    public:
    // Should be replaced by expr, when I get to those
    literal_node* val;
    virtual std::string dump(int);
};

class func_call_stmt : public stmt
{
    public:
    identifier ident;
    arguments args;
    virtual std::string dump(int);
};