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
    std::vector<std::shared_ptr<ast_node>> children;
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

class parameters : public ast_node
{
    public:
    //std::vector<std::shared_ptr<>> body;
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
    dtype type;
    identifier ident;
    virtual std::string dump(int) override;
};

class def_stmt : public stmt
{
    public:
    identifier ident;
    std::shared_ptr<expr> value;
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
    std::shared_ptr<expr> val;
    virtual std::string dump(int) override;
};

// Like a function declaration but without the code
class func_def_stmt : public stmt
{
    public:
    virtual std::string dump(int) override;
};

// Declares(and probably defines too) a function
class func_decl_stmt : public stmt
{
    public:
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
    std::vector<func_decl_stmt> fdecls;
    std::vector<func_def_stmt> fdefs;
    entry_stmt entry;
    std::string dump();
};