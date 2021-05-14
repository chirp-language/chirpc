/// \file Abstract Spaghetti Noodles code representation

#pragma once
/*
All possible AST nodes are defined here
*/
#include "types.hpp"
#include "../shared/location_provider.hpp"
#include <string>
#include <vector>
#include <memory>
// === SHARED ===

class ast_node
{
public:
    location_range loc;
};

// Forward declarations
class expr;
class binop;
class arguments;
class func_call;
class identifier;
class loperand;
class literal_node;
class txt_literal;
class num_literal;
class decl;
class var_decl;
class entry_decl;
class parameters;
class func_decl;
class func_def;
class stmt;
class decl_stmt;
class assign_stmt;
class compound_stmt;
class ret_stmt;
class conditional_stmt;
class expr_stmt;
class ast_root;

// Node handles (shorthands)
using exprh = std::shared_ptr<expr>;
using declh = std::shared_ptr<decl>;
using stmth = std::shared_ptr<stmt>;

// === Expressions ===
// They take values and spit out other values.

// Operand type
enum class optype
{
    lit,
    ident,
    call,
    op,
    invalid,
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
};

class expr : public ast_node
{
    public:
    exprtype type;
    optype kind;

protected:
    expr(optype kind)
        : type{}, kind(kind) {}
};

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

    binop(exprop op, exprh l, exprh r)
        : expr(optype::op), op(op), left(std::move(l)), right(std::move(r)) {}
};

class arguments : public ast_node
{
public:
    std::vector<exprh> body;
};

class func_call : public expr
{
public:
    exprh callee;
    arguments args;

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

    identifier() : expr(optype::ident) {}
};

class loperand : public expr
{
    public:
    std::shared_ptr<ast_node> node;
    loptype type;
};

enum class littype
{
    txt,
    num,
};

class literal_node : public expr
{
public:
    littype ltype;

protected:
    literal_node(littype t) : expr(optype::lit), ltype(t) {}
};

class txt_literal : public literal_node
{
public:
    bool is_character; // Single character/packed character constant
    std::string value;

    txt_literal() : literal_node(littype::txt) {}
};

// Should be replaced by a different literal for each types
class num_literal : public literal_node
{
public:
    std::string value; // bruh

    num_literal() : literal_node(littype::num) {}
};

// === Declarations ===

enum class decl_type
{
    var, entry, import,
    fdecl, fdef,
    external,
};

class decl : public ast_node
{
public:
    decl_type type;

protected:
    decl(decl_type type) : type(type) {}
};

class var_decl : public decl
{
    public:
    exprtype var_type;
    std::shared_ptr<identifier> ident;
    exprh init; // Empty if not availible

    var_decl() : decl(decl_type::var) {}
};

class entry_decl : public decl
{
public:
    stmth code;

    entry_decl() : decl(decl_type::entry) {}
};

class import_decl : public decl
{
public:
    txt_literal filename;

    import_decl() : decl(decl_type::import) {}
};

class extern_decl : public decl
{
    public:
    txt_literal real_name;
    declh inner_decl;

    extern_decl() : decl(decl_type::external) {}
};

class parameters : public ast_node
{
    public:
    std::vector<std::shared_ptr<var_decl>> body;
};

// Like a function definition but without the code
class func_decl : public decl
{
    public:
    exprtype data_type;
    std::shared_ptr<identifier> ident;
    parameters params;

    func_decl() : decl(decl_type::fdecl) {}

    protected:
    func_decl(decl_type t) : decl(t) {}
};

// A function definition but with code
class func_def : public func_decl
{
    public:
    std::shared_ptr<compound_stmt> body;

    func_def() : func_decl(decl_type::fdef) {}
};

// === Statements ===

enum class stmt_type
{
    decl, assign,
    compound, ret,
    conditional, expr,
};

class stmt : public ast_node
{
public:
    // There's a bunch of them so gotta make another enum smh
    stmt_type type;

protected:
    stmt(stmt_type type) : type(type) {}
};

class decl_stmt : public stmt
{
    public:
    declh inner_decl;

    decl_stmt(declh inner)
        : stmt(stmt_type::decl), inner_decl(std::move(inner))
    {
        loc = inner_decl->loc;
    }

    static std::shared_ptr<decl_stmt> from(declh decl) {
        return std::make_shared<decl_stmt>(std::move(decl));
    }
};

class assign_stmt : public stmt
{
    public:
    std::shared_ptr<identifier> ident;
    var_decl* target = nullptr;
    exprh value;

    assign_stmt() : stmt(stmt_type::assign) {}
};

class compound_stmt : public stmt
{
public:
    std::vector<stmth> body;

    compound_stmt() : stmt(stmt_type::compound) {}
};

class ret_stmt : public stmt
{
public:
    exprh val;

    ret_stmt() : stmt(stmt_type::ret) {}
};

class conditional_stmt : public stmt
{
    public:
    exprh cond;
    stmth true_branch;
    stmth false_branch; // Can be null

    conditional_stmt() : stmt(stmt_type::conditional) {}
};

class expr_stmt : public stmt
{
    public:
    exprh node;

    expr_stmt(exprh expr)
        : stmt(stmt_type::expr), node(std::move(expr))
    {
        loc = node->loc;
    }

    static std::shared_ptr<expr_stmt> from(exprh expr) {
        return std::make_shared<expr_stmt>(std::move(expr));
    }
};

class ast_root
{
public:
    // Vectors are in order
    std::vector<std::shared_ptr<import_decl>> imports;
    std::vector<std::shared_ptr<extern_decl>> externs;
    std::vector<std::shared_ptr<func_decl>> fdecls;
    std::vector<std::shared_ptr<func_def>> fdefs;
    std::shared_ptr<entry_decl> entry;
};
