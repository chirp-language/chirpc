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
#include <memory>
// === SHARED ===

class ast_node
{
public:
    virtual std::string dump(int);
    std::vector<std::shared_ptr<ast_node>> children;
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
    virtual std::string dump(int) override;
};

class literal_node : public ast_node
{
public:
    virtual std::string dump(int) override;
};

class txt_literal : public literal_node
{
public:
    virtual std::string dump(int) override;
    bool single_char;
    std::string value;
};

// Should be replaced by a different literal for each types
class num_literal : public literal_node
{
public:
    std::string value;
    virtual std::string dump(int) override;
};

// === Expression Stuff ===

class mathop : public ast_node
{
    public:
    char type;
    num_literal left;
    num_literal right;
    virtual std::string dump(int) override;
};

class expr : public ast_node
{
    public:
    virtual std::string dump(int) override;
};

class mathexpr : public expr
{
    public:
    // Litteraly just the same char
    std::vector<mathop> operands;
    virtual std::string dump(int) override;
};

class staticexpr : public expr
{
    public:
    std::shared_ptr<literal_node> value;
    virtual std::string dump(int) override;
};

// === Stuff that likes has literals in it but not a statement ish

class arguments : public ast_node
{
public:
    std::vector<std::shared_ptr<expr>> body;
    virtual std::string dump(int) override;
};

// === STATEMENTS ===

class stmt : public ast_node
{
public:
    // It's kindof a location-ish
    int line;
    virtual std::string dump(int) override;
};

class compound_stmt : public stmt
{
public:
    std::vector<std::shared_ptr<stmt>> body;
    virtual std::string dump(int) override;
};

class entry_stmt : public stmt
{
public:
    bool has_args;
    // Arguments may not exist
    std::shared_ptr<arguments> args;

    std::shared_ptr<stmt> code;
    virtual std::string dump(int) override;
};

class import_stmt : public stmt
{
public:
    txt_literal filename;
    virtual std::string dump(int) override;
};

class ret_stmt : public stmt
{
public:
    // Should be replaced by expr, when I get to those
    std::shared_ptr<expr> val;
    virtual std::string dump(int) override;
};

class func_call_stmt : public stmt
{
public:
    identifier ident;
    arguments args;
    virtual std::string dump(int) override;
};