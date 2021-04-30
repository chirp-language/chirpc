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
    virtual std::string dump(int, location_provider const&) const = 0;
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

    std::string dump(int depth, location_provider const&) const;
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
    std::string dump(int depth, location_provider const&) const override;

    binop(exprop op, exprh l, exprh r)
        : expr(optype::op), op(op), left(std::move(l)), right(std::move(r)) {}
};

class arguments : public ast_node
{
public:
    std::vector<exprh> body;
    std::string dump(int depth, location_provider const&) const override;
};

class func_call : public expr
{
public:
    exprh callee;
    arguments args;
    std::string dump(int depth, location_provider const&) const override;

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
    std::string dump(int depth, location_provider const&) const override;

    identifier() : expr(optype::ident) {}
};

class loperand : public expr
{
    public:
    std::shared_ptr<ast_node> node;
    loptype type;
    std::string dump(int depth, location_provider const&) const override;
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
    std::string dump(int depth, location_provider const&) const override;

    literal_node(littype t) : expr(optype::lit), ltype(t) {}
};

class txt_literal : public literal_node
{
public:
    std::string dump(int depth, location_provider const&) const override;
    bool is_character; // Single character/packed character constant
    std::string value;

    txt_literal() : literal_node(littype::txt) {}
};

// Should be replaced by a different literal for each types
class num_literal : public literal_node
{
public:
    std::string value; // bruh
    std::string dump(int depth, location_provider const&) const override;

    num_literal() : literal_node(littype::num) {}
};

// === STATEMENTS ===

enum class stmt_type
{
    decl, def,
    compound, entry, import,
    ret, fdef, fdecl, expr,
    external,
};

class stmt : public ast_node
{
public:
    // There's a bunch of them so gotta make another enum smh
    stmt_type type;
    std::string dump(int depth, location_provider const&) const override;

    stmt(stmt_type type)
        : type(type) {}
};

// Statement handle (shorthand)
using stmth = std::shared_ptr<stmt>;

class decl_stmt : public stmt
{
    public:
    exprtype data_type;
    std::shared_ptr<identifier> ident;
    exprh init; // Empty if not availible
    std::string dump(int depth, location_provider const&) const override;

    decl_stmt() : stmt(stmt_type::decl) {}
};

class def_stmt : public stmt
{
    public:
    std::shared_ptr<identifier> ident;
    exprh value;
    std::string dump(int depth, location_provider const&) const override;

    def_stmt() : stmt(stmt_type::def) {}
};

class compound_stmt : public stmt
{
public:
    std::vector<stmth> body;
    std::string dump(int depth, location_provider const&) const override;

    compound_stmt() : stmt(stmt_type::compound) {}
};

class entry_stmt : public stmt
{
public:
    stmth code;
    std::string dump(int depth, location_provider const&) const override;

    entry_stmt() : stmt(stmt_type::entry) {}
};

class import_stmt : public stmt
{
public:
    txt_literal filename;
    std::string dump(int depth, location_provider const&) const override;

    import_stmt() : stmt(stmt_type::import) {}
};

class ret_stmt : public stmt
{
public:
    // Should be replaced by expr, when I get to those
    exprh val;
    std::string dump(int depth, location_provider const&) const override;

    ret_stmt() : stmt(stmt_type::ret) {}
};

class extern_stmt : public stmt
{
    public:
    enum class decl_type
    {
        None,
        Function,
        Variable
    } type;
    txt_literal real_name;
    stmth decl;
    std::string dump(int depth, location_provider const&) const override;

    extern_stmt()
        : stmt(stmt_type::external), type(decl_type::None) {}
};

class parameters : public ast_node
{
    public:
    std::vector<std::shared_ptr<decl_stmt>> body;
    std::string dump(int depth, location_provider const&) const override;
};

// Like a function definition but without the code
class func_decl_stmt : public stmt
{
    public:
    exprtype data_type;
    std::shared_ptr<identifier> ident;
    parameters params;
    std::string dump(int depth, location_provider const&) const override;

    func_decl_stmt() : stmt(stmt_type::fdecl) {}
};

// A function definition but with code
class func_def_stmt : public stmt
{
    public:
    exprtype data_type;
    std::shared_ptr<identifier> ident;
    parameters params;
    std::shared_ptr<compound_stmt> body;
    std::string dump(int depth, location_provider const&) const override;

    func_def_stmt() : stmt(stmt_type::fdef) {}
};

class expr_stmt : public stmt
{
    public:
    exprh node;
    std::string dump(int depth, location_provider const&) const override;

    expr_stmt(exprh expr)
        : stmt(stmt_type::expr), node(std::move(expr)) {}
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
    std::string dump(int depth, location_provider const&) const;
};
