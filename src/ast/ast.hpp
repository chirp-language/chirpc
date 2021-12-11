/// \file AST Nodes - Abstract SpaghetTi Noodles code representation

#pragma once
/*
All possible AST nodes are defined here
*/
#include "types.hpp"
#include "../shared/location_provider.hpp"
#include "../shared/system.hpp"
#include <string>
#include <memory>
// === SHARED ===

class ast_node
{
public:
    location_range loc;
};

// Forward declarations
class tracker_symbol;

class ast_root;
class identifier;
class raw_qual_identifier;
class qual_identifier;
class expr;
class binop;
class unop;
class arguments;
class func_call;
class id_ref_expr;
class loperand;
class string_literal;
class integral_literal;
class nullptr_literal;
class cast_expr;
class decl;
class var_decl;
class entry_decl;
class import_decl;
class extern_decl;
class namespace_decl;
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

class identifier : public ast_node
{
    public:
    std::string name;

    static identifier from(std::string&& name, token_location loc = token_location()) {
        identifier i;
        i.name = std::move(name);
        i.loc = loc;
        return i;
    }
};

class raw_qual_identifier
{
    public:
    // The parts vector for a.b.c.foo() would be:
    // {"a","b","c","foo"}.. Further in vector => More nested
    std::vector<identifier> parts;
};

class qual_identifier : public ast_node, public raw_qual_identifier
{
    public:
    bool is_global = false; // Start at global namespace
};

// === Expressions ===
// They take values and spit out other values.

enum class expr_kind
{
    binop,
    unop,
    call,
    ident,
    strlit,
    intlit,
    nulllit,
    cast,
};

class expr : public ast_node
{
    public:
    using node_base = expr; // For node type identification

    expr_kind kind;
    basic_type type;
    exprcat cat;

    bool has_error() const
    {
        return cat == exprcat::error;
    }

    protected:
    expr(expr_kind kind)
        : kind(kind), cat(exprcat::unset) {}
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
        : expr(expr_kind::binop), op(op), left(std::move(l)), right(std::move(r)) {}
};

// (Prefix) unary operator
class unop : public expr
{
    public:
    tkn_type op;
    token_location op_loc;
    exprh operand;

    unop(tkn_type op, exprh v)
        : expr(expr_kind::unop), op(op), operand(std::move(v)) {}
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
        : expr(expr_kind::call), callee(std::move(callee)), args(std::move(args)) {}
};

class id_ref_expr : public expr
{
    public:
    qual_identifier ident;
    decl const* target = nullptr;

    id_ref_expr(qual_identifier ident)
        : expr(expr_kind::ident), ident(std::move(ident))
    {
        loc = ident.loc;
    }

    static nodeh<id_ref_expr> from(qual_identifier ident)
    {
        return new_node<id_ref_expr>(std::move(ident));
    }
};

class string_literal : public expr
{
    public:
    std::string value;

    string_literal() : expr(expr_kind::strlit) {}
};

struct integer_value
{
    int64_t val;

    integer_value() = default;
    constexpr integer_value(int64_t v) : val(v) {}
};

class integral_literal : public expr
{
    public:
    integer_value value;

    integral_literal() : expr(expr_kind::intlit) {}

    protected:
    integral_literal(expr_kind k) : expr(k) {}
};

class nullptr_literal : public expr
{
    public:
    nullptr_literal() : expr(expr_kind::nulllit) {}
};

enum class cast_kind
{
    _invalid,  // Invalid cast operation
    _explicit, // Cast spelled in the program, (expression) as(type)
    _const,    // Cast between const-qualified and non-const-qualified types
    _grade,    // Cast between types of different sizes/characteristics
    _sign,     // Cast between integral types of different signs (signed, unsigned, unspecified)
    _cat,      // Value category conversion
    _float,    // Floating point conversion, between floats and ints
    _bool,     // Boolean conversion
};

class cast_expr : public expr
{
    public:
    exprh operand;
    cast_kind ckind;

    cast_expr(cast_kind ckind)
        : expr(expr_kind::cast), ckind(ckind) {}
};

// === Declarations ===

enum class decl_kind
{
    root,
    var,
    entry,
    import,
    nspace,
    fdecl,
    fdef,
    external,
};

class decl : public ast_node
{
    public:
    using node_base = decl; // For node type identification

    decl(decl const&) = delete;
    decl& operator=(decl const&) = delete;

    decl_kind kind;
    tracker_symbol* symbol = nullptr; // Assiociated symbol

    protected:
    decl(decl_kind kind) : kind(kind) {}
};

class ast_root : public decl
{
public:
    #if 0
    // Vectors are in order
    std::vector<std::unique_ptr<import_decl>> imports;
    std::vector<std::unique_ptr<extern_decl>> externs;
    std::vector<std::unique_ptr<namespace_decl>> nspaces;
    std::vector<std::unique_ptr<func_decl>> fdecls;
    std::vector<std::unique_ptr<func_def>> fdefs;
    std::unique_ptr<entry_decl> entry;
    #endif
    std::vector<declh> top_decls;
    entry_decl* entry = nullptr;

    ast_root() : decl(decl_kind::root) {}
};

class var_decl : public decl
{
    public:
    identifier ident;
    basic_type type;
    exprh init; // Empty if not availible

    var_decl() : decl(decl_kind::var) {}
};

class entry_decl : public decl
{
    public:
    stmth code;

    entry_decl() : decl(decl_kind::entry) {}
};

class import_decl : public decl
{
    public:
    std::string filename;

    import_decl() : decl(decl_kind::import) {}
};

class extern_decl : public decl
{
    public:
    std::string real_name;
    token_location name_loc;
    declh inner_decl;

    extern_decl() : decl(decl_kind::external) {}
};

class namespace_decl : public decl
{
    public:
    identifier ident;

    std::vector<declh> decls;

    namespace_decl() : decl(decl_kind::nspace) {}
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
    identifier ident;
    basic_type result_type;
    parameters params;

    func_decl() : decl(decl_kind::fdecl) {}

    protected:
    func_decl(decl_kind t) : decl(t) {}
};

// A function definition but with code
class func_def : public func_decl
{
    public:
    std::unique_ptr<compound_stmt> body;

    func_def() : func_decl(decl_kind::fdef) {}
};

// === Statements ===

enum class stmt_kind
{
    decl,
    assign,
    compound,
    ret,
    conditional,
    iteration,
    expr,
    null,
};

class stmt : public ast_node
{
    public:
    using node_base = stmt; // For node type identification

    stmt(stmt const&) = delete;
    stmt& operator=(stmt const&) = delete;

    // There's a bunch of them so gotta make another enum smh
    stmt_kind kind;

    protected:
    stmt(stmt_kind kind) : kind(kind) {}
};

class decl_stmt : public stmt
{
    public:
    declh inner_decl;

    decl_stmt(declh inner)
        : stmt(stmt_kind::decl), inner_decl(std::move(inner))
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
    exprh target;
    exprh value;
    tkn_type assign_op; // Type of assignment
    token_location assign_loc; // location of '=', or compound assignment token

    assign_stmt() : stmt(stmt_kind::assign) {}
};

class compound_stmt : public stmt
{
    public:
    std::vector<stmth> body;

    compound_stmt() : stmt(stmt_kind::compound) {}
};

class ret_stmt : public stmt
{
    public:
    exprh val;

    ret_stmt() : stmt(stmt_kind::ret) {}
};

class conditional_stmt : public stmt
{
    public:
    exprh cond;
    stmth true_branch;
    stmth false_branch; // Can be null

    conditional_stmt() : stmt(stmt_kind::conditional) {}
};

class iteration_stmt : public stmt
{
    public:
    exprh cond;
    stmth loop_body;

    iteration_stmt() : stmt(stmt_kind::iteration) {}
};

class expr_stmt : public stmt
{
    public:
    exprh node;

    expr_stmt(exprh expr)
        : stmt(stmt_kind::expr), node(std::move(expr))
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
        : stmt(stmt_kind::null)
    {
        this->loc = loc;
    }
};
