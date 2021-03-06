#pragma once
/*
All possible AST nodes are defined here
*/
#include "types.hpp"
#include <string>
#include <vector>
#include <memory>
// === SHARED ===

class ast_node
{
public:
    virtual std::string dump(int);
    //std::vector<std::shared_ptr<ast_node>> children;
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

enum class littype
{
    txt,
    num
};

class literal_node : public ast_node
{
public:
    littype ltype;
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
// I am spoon brain so I just like, improvised expression parsing..
// Cuz pfff what is Shunting-yard ???! All I know is Bink's-Yard amirite

// Operand type
enum class optype
{
    lit,
    ident,
    call,
    subexpr,
    op, // just used during parsing
    invalid
};


// Operator
class exprop : public ast_node
{
    public:
    char type;
    //bool binary;
    virtual std::string dump(int) override;
};

class operand : public ast_node
{
    public:
    optype type;
    std::shared_ptr<ast_node> node;
    virtual std::string dump(int) override;
};

class subexpr : public ast_node
{
    public:
    exprop op;
    operand left;
    operand right;
    virtual std::string dump(int) override;
};

class expr : public ast_node
{
    public:
    operand root;
    virtual std::string dump(int) override;
};

// === Stuff that likes has literals in it but not a statement ish

class arguments : public ast_node
{
public:
    std::vector<expr> body;
    virtual std::string dump(int) override;
};

// === STATEMENTS ===

enum class stmt_type
{
    decl, def, decldef,
    compound, entry, import, 
    ret, fdef, fdecl, fcall
};

class stmt : public ast_node
{
public:
    // There's a bunch of them so gotta make another enum smh
    stmt_type type;
    // It's kindof a location-ish
    int line;
    virtual std::string dump(int) override;
};

class dtype : public ast_node
{
    public:
    dtypename tname;
    std::vector<char> tmods; // Enums are cast to/from a char
    virtual std::string dump(int) override;
};

class decl_stmt : public stmt
{
    public:
    dtype data_type;
    identifier ident;
    virtual std::string dump(int) override;
};

class def_stmt : public stmt
{
    public:
    identifier ident;
    expr value;
    virtual std::string dump(int) override;
};

class decldef_stmt : public stmt
{
    public:
    decl_stmt decl;
    def_stmt def;
    virtual std::string dump(int override);
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
    expr val;
    virtual std::string dump(int) override;
};

class extern_stmt : public stmt
{
    public:
    enum class stmt_type
    {
        None,
        Function,
        Variable
    }type;
    txt_literal real_name;
    std::shared_ptr<stmt> stmt;
    virtual std::string dump(int) override;
};

class parameters : public ast_node
{
    public:
    std::vector<decl_stmt> body;
    virtual std::string dump(int) override;
};

// Like a function definition but without the code
class func_decl_stmt : public stmt
{
    public:
    dtype data_type;
    identifier ident;
    parameters params;
    virtual std::string dump(int) override;
};

// A function definition but with code
class func_def_stmt : public stmt
{
    public:
    dtype data_type;
    identifier ident;
    parameters params;
    compound_stmt body;
    virtual std::string dump(int) override;
};

class func_call_stmt : public stmt
{
public:
    identifier ident;
    arguments args;
    virtual std::string dump(int) override;
};

class ast
{
public:
    // Vectors are in order
    std::vector<import_stmt> imports;
    std::vector<extern_stmt> externs;
    std::vector<func_decl_stmt> fdecls;
    std::vector<func_def_stmt> fdefs;
    bool has_entry = false;
    entry_stmt entry;
    std::string dump();
};