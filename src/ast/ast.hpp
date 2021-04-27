/// \file Abstract Spaghetti Noodles code representation

#pragma once
/*
All possible AST nodes are defined here
*/
#include "types.hpp"
#include "../lexer/token.hpp"
#include <string>
#include <vector>
#include <memory>
// === SHARED ===

class ast_node
{
public:
    virtual std::string dump(int) = 0;
    //std::vector<std::shared_ptr<ast_node>> children;

    location_range loc;
};

// === LITERALS & SIMILAR??! ===

// === Expression Stuff ===
// I am spoon brain so I just like, improvised expression parsing..
// Cuz pfff what is Shunting-yard ???! All I know is Bink's-Yard amirite

// Operand type
enum class optype
{
    lit,
    ident,
    call,
    op,
    invalid
};

enum class opcat
{
    unset = 1, // unknown/unassigned type
    lval = 2, // lvalue, i.e. has location
    pval = 4, // pvalue, i.e. a loose value (not tied to any object)
};

struct exprtype
{
    dtypename basetp;
    opcat cattp;
    std::vector<std::byte> exttp; // Enums are cast to/from a byte bc why not

    std::string dump(int depth) const;
};

class expr : public ast_node
{
    public:
    exprtype type;
    optype kind;

    expr(optype kind)
        : type{}, kind(kind) {}
};

// Expression handle (shorthand)
using exprh = std::shared_ptr<expr>;


// Operator
enum class exprop : short {
    none = 0,
    as = -1,
    deref = -2,
    ref = -3,
    call = -4,
};

std::string exprop_id(exprop op);

class binop : public expr
{
    public:
    exprop op;
    token_location op_loc;
    exprh left;
    exprh right;
    virtual std::string dump(int) override;

    binop(exprop op, exprh l, exprh r)
        : expr(optype::op), op(op), left(std::move(l)), right(std::move(r)) {}
};

class func_call : public expr
{
public:
    exprh callee;
    arguments args;
    virtual std::string dump(int) override;

    func_call(exprh callee, arguments args)
        : expr(optype::call), callee(std::move(callee)), args(std::move(args)) {}
};

// Left-Side stuff

// Left Side Operand type

enum class loptype
{
    access, // Array Accessor []
    ident,
    lunop, // left unary operator
};

class identifier : public expr
{
public:
    // The namespace vector for a.b.c.foo() would be like:
    // {"a","b","c"}.. Further in vector => More nested
    std::vector<std::string> namespaces;
    std::string name;
    virtual std::string dump(int) override;
};

class loperand : public expr
{
    public:
    std::shared_ptr<ast_node> node;
    loptype type;
    virtual std::string dump(int) override;
};

enum class littype
{
    txt,
    num
};

class literal_node : public expr
{
public:
    littype ltype;
    virtual std::string dump(int) override;
};

class txt_literal : public literal_node
{
public:
    virtual std::string dump(int) override;
    bool is_character; // Single character/packed character constant
    std::string value;
};

// Should be replaced by a different literal for each types
class num_literal : public literal_node
{
public:
    std::string value; // bruh
    virtual std::string dump(int) override;
};

// === Stuff that likes has literals in it but not a statement ish

class arguments : public ast_node
{
public:
    std::vector<exprh> body;
    virtual std::string dump(int) override;
};

// === STATEMENTS ===

enum class stmt_type
{
    decl, def, decldef,
    compound, entry, import,
    ret, fdef, fdecl, expr
};

class stmt : public ast_node
{
public:
    // There's a bunch of them so gotta make another enum smh
    stmt_type type;
    virtual std::string dump(int) override;
};

// Statement handle (shorthand)
using stmth = std::shared_ptr<stmt>;

class decl_stmt : public stmt
{
    public:
    exprtype data_type;
    std::shared_ptr<identifier> ident;
    exprh init; // Empty if not availible
    virtual std::string dump(int) override;
};

class def_stmt : public stmt
{
    public:
    std::shared_ptr<identifier> ident;
    exprh value;
    virtual std::string dump(int) override;
};

// Combines a declaration with a definition
class decl_def_stmt : public stmt
{
    public:
    std::shared_ptr<decl_stmt> decl;
    std::shared_ptr<def_stmt> def;
    virtual std::string dump(int) override;
};

class compound_stmt : public stmt
{
public:
    std::vector<stmth> body;
    virtual std::string dump(int) override;
};

class entry_stmt : public stmt
{
public:
    stmth code;
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
    exprh val;
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
    } type;
    txt_literal real_name;
    stmth stmt;
    virtual std::string dump(int) override;
};

class parameters : public ast_node
{
    public:
    std::vector<std::shared_ptr<decl_stmt>> body;
    virtual std::string dump(int) override;
};

// Like a function definition but without the code
class func_decl_stmt : public stmt
{
    public:
    exprtype data_type;
    std::shared_ptr<identifier> ident;
    parameters params;
    virtual std::string dump(int) override;
};

// A function definition but with code
class func_def_stmt : public stmt
{
    public:
    exprtype data_type;
    std::shared_ptr<identifier> ident;
    parameters params;
    std::shared_ptr<compound_stmt> body;
    virtual std::string dump(int) override;
};

class expr_stmt : public stmt
{
    public:
    exprh node;
    virtual std::string dump(int) override;
};

class ast_root
{
public:
    // Vectors are in order
    std::vector<std::shared_ptr<import_stmt>> imports;
    std::vector<std::shared_ptr<extern_stmt>> externs;
    std::vector<std::shared_ptr<func_decl_stmt>> fdecls;
    std::vector<std::shared_ptr<func_def_stmt>> fdefs;
    bool has_entry = false;
    entry_stmt entry;
    std::string dump();
};
