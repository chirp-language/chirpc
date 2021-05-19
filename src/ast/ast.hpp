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
class ast_root;
class identifier;
class expr;
class binop;
class arguments;
class func_call;
class id_ref_expr;
class loperand;
class literal_node;
class txt_literal;
class num_literal;
class decl;
class var_decl;
class entry_decl;
class import_decl;
class extern_decl;
class parameters;
class func_decl;
class func_def;
class stmt;
class decl_stmt;
class assign_stmt;
class compound_stmt;
class ret_stmt;
class conditional_stmt;
class iteration_stmt;
class expr_stmt;
class null_stmt;

// Node deleters
struct expr_node_deleter {
    void operator()(expr*) const;

    template <typename T, typename = std::enable_if_t<std::is_same_v<typename T::node_base, expr>>>
    operator std::default_delete<T>() const
    { return {}; }
};

struct decl_node_deleter {
    void operator()(decl*) const;

    template <typename T, typename = std::enable_if_t<std::is_same_v<typename T::node_base, decl>>>
    operator std::default_delete<T>() const
    { return {}; }
};

struct stmt_node_deleter {
    void operator()(stmt*) const;

    template <typename T, typename = std::enable_if_t<std::is_same_v<typename T::node_base, stmt>>>
    operator std::default_delete<T>() const
    { return {}; }
};

template <typename T>
struct node_deleter_for_
{
    template <typename>
    constexpr static bool dependent_false = false;
    static_assert(dependent_false<T>, "Generic handle cannot be used for other node types");
};

template <>
struct node_deleter_for_<expr> { using type = expr_node_deleter; };

template <>
struct node_deleter_for_<decl> { using type = decl_node_deleter; };

template <>
struct node_deleter_for_<stmt> { using type = stmt_node_deleter; };

// Generic node handle
template <typename T>
using generic_node_deleter = typename node_deleter_for_<typename T::node_base>::type;

template <typename T>
using nodeh = std::unique_ptr<T, generic_node_deleter<T>>;

template <typename T, typename... Args>
inline nodeh<T> new_node(Args&&... args)
{
    return nodeh<T>(new T(std::forward<Args>(args)...));
}

// Node handles (shorthands)
using exprh = std::unique_ptr<expr, expr_node_deleter>;
using declh = std::unique_ptr<decl, decl_node_deleter>;
using stmth = std::unique_ptr<stmt, stmt_node_deleter>;

// === Top level / Common ===

class ast_root
{
public:
    // Vectors are in order
    std::vector<std::unique_ptr<import_decl>> imports;
    std::vector<std::unique_ptr<extern_decl>> externs;
    std::vector<std::unique_ptr<func_decl>> fdecls;
    std::vector<std::unique_ptr<func_def>> fdefs;
    std::unique_ptr<entry_decl> entry;
};

class identifier : public ast_node
{
public:
    // The namespace vector for a.b.c.foo() would be like:
    // {"a","b","c"}.. Further in vector => More nested
    std::vector<std::string> namespaces;
    std::string name;
};

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
    unset, // unknown/unassigned type
    lval, // lvalue, i.e. has location
    rval, // rvalue, i.e. a pure value (not tied to any object)
    error, // result of an invalid operation
};

struct exprtype
{
    dtypename basetp;
    opcat cattp;
    std::vector<std::byte> exttp; // Enums are cast to/from a byte bc why not

    exprtype()
        : basetp(dtypename::_none), cattp(opcat::unset) {}
};

class expr : public ast_node
{
    public:
    using node_base = expr; // For node type identification

    exprtype type;
    optype kind;

    protected:
    expr(optype kind)
        : kind(kind) {}
};

// Operator
class binop : public expr
{
    public:
    tkn_type op;
    token_location op_loc;
    exprh left;
    exprh right;

    binop(tkn_type op, exprh l, exprh r)
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

class id_ref_expr : public expr
{
    public:
    identifier ident;
    decl const* target = nullptr;

    id_ref_expr(identifier ident)
        : expr(optype::ident), ident(std::move(ident))
    {
        loc = ident.loc;
    }

    static nodeh<id_ref_expr> from(identifier ident)
    {
        return new_node<id_ref_expr>(std::move(ident));
    }
};

#if 0
class loperand : public expr
{
    public:
    std::unique_ptr<ast_node> node;
    loptype type;
};
#endif

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
    using node_base = decl; // For node type identification

    decl_type type;

    protected:
    decl(decl_type type) : type(type) {}
};

class var_decl : public decl
{
    public:
    exprtype var_type;
    identifier ident;
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
    std::vector<std::unique_ptr<var_decl>> body;
};

// Like a function definition but without the code
class func_decl : public decl
{
    public:
    exprtype data_type;
    identifier ident;
    parameters params;

    func_decl() : decl(decl_type::fdecl) {}

    protected:
    func_decl(decl_type t) : decl(t) {}
};

// A function definition but with code
class func_def : public func_decl
{
    public:
    std::unique_ptr<compound_stmt> body;

    func_def() : func_decl(decl_type::fdef) {}
};

// === Statements ===

enum class stmt_type
{
    decl, assign,
    compound, ret,
    conditional,
    iteration, expr,
    null,
};

class stmt : public ast_node
{
    public:
    using node_base = stmt; // For node type identification

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

    static nodeh<decl_stmt> from(declh decl) {
        return new_node<decl_stmt>(std::move(decl));
    }
};

class assign_stmt : public stmt
{
    public:
    identifier ident;
    var_decl const* target = nullptr;
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

class iteration_stmt : public stmt
{
    public:
    exprh cond;
    stmth loop_body;

    iteration_stmt() : stmt(stmt_type::iteration) {}
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

    static nodeh<expr_stmt> from(exprh expr) {
        return new_node<expr_stmt>(std::move(expr));
    }
};

class null_stmt : public stmt
{
    public:
    null_stmt(location_range loc)
        : stmt(stmt_type::null)
    {
        this->loc = loc;
    }
};
