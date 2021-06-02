#include "ast_dumper.hpp"
#include "../color.hpp"

#include <iostream>
#include <queue>
#include <utility>

// === UTILS FUNCTIONS FOR THE UTIL AST DUMPS ===
// ^ meta

// Colors used by the AST dump
constexpr color c_color_top_level = color::blue | color::bright | color::bold;
constexpr color c_color_top_level_unavail = color::red | color::bright | color::bold;
constexpr color c_color_type = color::green;
constexpr color c_color_type_cat = color::green | color::blue;
constexpr color c_color_expr = color::blue | color::bright | color::bold;
constexpr color c_color_decl = color::green | color::bright | color::bold;
constexpr color c_color_stmt = color::red | color::blue | color::bright | color::bold;
constexpr color c_color_identifier = color::blue | color::bright;
constexpr color c_color_location = color::red | color::green;

static std::string indent(int x)
{
    std::string result;
    result.resize(x * 3, ' ');
    return result;
}

void text_ast_dumper::write_color(std::string txt, color c) {
    if (has_colors) {
        #ifdef __unix__
        // Doesn't care if it's on a VT100 terminal or not
        // will do coloring anyway.
        std::cout << "\033[";
        unsigned int col = static_cast<unsigned int>(c);
        if ((c & color::bright) != color::blank)
            std::cout << (90 + (col & 7));
        else
            std::cout << (30 + (col & 7));
        if ((c & color::bold) != color::blank)
            std::cout << ";1";
        std::cout << 'm';
        #endif
    }
    std::cout << txt;
    if (has_colors) {
        #ifdef __unix__
        std::cout << "\033[m";
        #endif
    }
}

void text_ast_dumper::print_location(location_range loc) {
    if (loc_prov)
        write_color(loc_prov->print_loc(loc), c_color_location);
    else
        write_color("<unavail>", c_color_location);
}

// I don't even care about names now
static std::string dump_dtname(dtypename n)
{
    switch (n)
    {
    case dtypename::_int:
        return "int";
    case dtypename::_float:
        return "float";
    case dtypename::_double:
        return "double";
    case dtypename::_char:
        return "char";
    case dtypename::_byte:
        return "byte";
    case dtypename::_bool:
        return "bool";
    case dtypename::_none:
        return "none";
    default:
        return "unknown";
    }
}

static std::string dump_dtmod(dtypemod m)
{
    switch (m)
    {
    case dtypemod::_ptr:
        return "ptr";
    case dtypemod::_signed:
        return "signed";
    case dtypemod::_unsigned:
        return "unsigned";
    case dtypemod::_const:
        return "const";
    case dtypemod::_func:
        return "func";
    default:
        return "<invalid mod>";
    }
}

static std::string dump_exprcat(exprcat c)
{
    switch (c)
    {
        case exprcat::unset:
            return "unset";
        case exprcat::lval:
            return "lvalue";
        case exprcat::rval:
            return "rvalue";
        case exprcat::error:
            return "error";
    }
    __builtin_unreachable();
}

std::string exprop_id(tkn_type op)
{
    switch (op)
    {
    case tkn_type::lt_op:
        return "<";
    case tkn_type::gt_op:
        return ">";
    case tkn_type::lteq_op:
        return "<=";
    case tkn_type::gteq_op:
        return ">=";
    case tkn_type::eqeq_op:
        return "==";
    case tkn_type::noteq_op:
        return "!=";
    case tkn_type::plus_op:
        return "+";
    case tkn_type::minus_op:
        return "-";
    case tkn_type::star_op:
        return "*";
    case tkn_type::slash_op:
        return "/";
    case tkn_type::perc_op:
        return "%";
    case tkn_type::as_op:
        return "as";
    case tkn_type::ref_op:
        return "ref";
    case tkn_type::deref_op:
        return "deref";
    // Assignments
    case tkn_type::assign_op:
        return "=";
    case tkn_type::compassign_op:
        // Placeholder
        return "@=";
    default:
        return "invalid";
    }
}

// AST util dumps

void text_ast_dumper::dump_ast(ast_root const& root)
{
    write_color("Top Level:\n", c_color_top_level);

    if (root.imports.empty())
    {
        write_color("-- No imports --\n", c_color_top_level_unavail);
    }
    else
    {
        for (auto& import : root.imports)
        {
            dump_import_decl(*import);
        }
    }

    if (root.externs.empty())
    {
        write_color("-- No externs --\n", c_color_top_level_unavail);
    }
    else
    {
        for (auto& ext : root.externs)
        {
            dump_extern_decl(*ext);
        }
    }

    if (root.fdecls.empty())
    {
        write_color("-- No function declarations on top-level --\n", c_color_top_level_unavail);
    }
    else
    {
        for (auto& node : root.fdecls)
        {
            dump_func_decl(*node);
        }
    }

    if (root.fdefs.empty())
    {
        write_color("-- No function definitions on top-level --\n", c_color_top_level_unavail);
    }
    else
    {
        for (auto& node : root.fdefs)
        {
            dump_func_def(*node);
        }
    }

    if (root.entry)
    {
        dump_entry_decl(*root.entry);
    }
    else
    {
        write_color("-- No entry --\n", c_color_top_level_unavail);
    }
}

void text_ast_dumper::dump_identifier(identifier const& n)
{
    std::cout << indent(depth);
    write_color("identifier ", c_color_identifier);
    print_location(n.loc);
    std::cout << " <";
    int i = 0;
    for (std::string const& wot : n.namespaces)
    {
        std::cout << wot;
        if (i != n.namespaces.size() - 1)
        {
            std::cout << ".";
        }
    }
    std::cout << "> ";
    std::cout << n.name;
    std::cout << '\n';
}

// Dispatch functions

void text_ast_dumper::dump_expr(expr const& node)
{
    switch (node.kind)
    {
        case expr_kind::binop:
            return dump_binop(static_cast<binop const&>(node));
        case expr_kind::call:
            return dump_func_call(static_cast<func_call const&>(node));
        case expr_kind::ident:
            return dump_id_ref_expr(static_cast<id_ref_expr const&>(node));
        case expr_kind::txtlit:
            return dump_txt_literal(static_cast<txt_literal const&>(node));
        case expr_kind::numlit:
            return dump_num_literal(static_cast<num_literal const&>(node));
        case expr_kind::cast:
            return dump_cast_expr(static_cast<cast_expr const&>(node));
        default:
            return;
    }
}

void text_ast_dumper::dump_decl(decl const& node)
{
    switch (node.kind)
    {
        case decl_kind::var:
            return dump_var_decl(static_cast<var_decl const&>(node));
        case decl_kind::entry:
            return dump_entry_decl(static_cast<entry_decl const&>(node));
        case decl_kind::import:
            return dump_import_decl(static_cast<import_decl const&>(node));
        case decl_kind::fdecl:
            return dump_func_decl(static_cast<func_decl const&>(node));
        case decl_kind::fdef:
            return dump_func_def(static_cast<func_def const&>(node));
        case decl_kind::external:
            return dump_extern_decl(static_cast<extern_decl const&>(node));
        default:
            return;
    }
}

void text_ast_dumper::dump_stmt(stmt const& node)
{
    switch (node.kind)
    {
        case stmt_kind::decl:
            return dump_decl_stmt(static_cast<decl_stmt const&>(node));
        case stmt_kind::assign:
            return dump_assign_stmt(static_cast<assign_stmt const&>(node));
        case stmt_kind::compound:
            return dump_compound_stmt(static_cast<compound_stmt const&>(node));
        case stmt_kind::ret:
            return dump_ret_stmt(static_cast<ret_stmt const&>(node));
        case stmt_kind::conditional:
            return dump_conditional_stmt(static_cast<conditional_stmt const&>(node));
        case stmt_kind::iteration:
            return dump_iteration_stmt(static_cast<iteration_stmt const&>(node));
        case stmt_kind::expr:
            return dump_expr_stmt(static_cast<expr_stmt const&>(node));
        case stmt_kind::null:
            return dump_null_stmt(node);
        default:
            return;
    }
}

// Expressions

void text_ast_dumper::dump_basic_type(basic_type const& type)
{
    std::cout << indent(depth);
    std::cout << "basic_type ";
    for (std::byte x : type.exttp)
    {
        dtypemod w = static_cast<dtypemod>(x);
        write_color(dump_dtmod(w), c_color_type);
        std::cout << ' ';
    }
    write_color(dump_dtname(type.basetp), c_color_type);
    std::cout << ";\n";
}

void text_ast_dumper::dump_expr_type(basic_type const& type, exprcat cat)
{
    std::cout << indent(depth);
    std::cout << "expr_type ";
    write_color(dump_exprcat(cat), c_color_type_cat);
    std::cout << ' ';
    for (std::byte x : type.exttp)
    {
        dtypemod w = static_cast<dtypemod>(x);
        write_color(dump_dtmod(w), c_color_type);
        std::cout << ' ';
    }
    write_color(dump_dtname(type.basetp), c_color_type);
    std::cout << ";\n";
}

void text_ast_dumper::dump_binop(binop const& n)
{
    std::cout << indent(depth);
    write_color("binop ", c_color_expr);
    print_location(n.loc);
    std::cout << " (";
    std::cout << exprop_id(n.op);
    std::cout << ' ';
    print_location(n.op_loc);
    std::cout << ")\n";
    ++depth;
    if (show_expr_types)
        dump_expr_type(n.type, n.cat);
    dump_expr(*n.left);
    dump_expr(*n.right);
    --depth;
}

void text_ast_dumper::dump_arguments(arguments const& n)
{
    std::cout << indent(depth);
    write_color("arguments ", c_color_expr);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    for (auto& n : n.body)
    {
        dump_expr(*n);
    }
    --depth;
}

void text_ast_dumper::dump_func_call(func_call const& n)
{
    std::cout << indent(depth);
    write_color("function_call ", c_color_expr);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    if (show_expr_types)
        dump_expr_type(n.type, n.cat);
    dump_expr(*n.callee);
    dump_arguments(n.args);
    --depth;
}

void text_ast_dumper::dump_id_ref_expr(id_ref_expr const& n)
{
    std::cout << indent(depth);
    write_color("id_ref_expr ", c_color_expr);
    print_location(n.loc);
    std::cout << " <";
    int i = 0;
    for (std::string const& wot : n.ident.namespaces)
    {
        std::cout << wot;
        if (i != n.ident.namespaces.size() - 1)
        {
            std::cout << ".";
        }
    }
    std::cout << "> ";
    std::cout << n.ident.name;
    std::cout << '\n';
    if (show_expr_types)
    {
        ++depth;
        dump_expr_type(n.type, n.cat);
        --depth;
    }
}

/*void text_ast_dumper::dump_loperand(loperand const& n)
{
    std::cout << indent(depth);
    write_color("left_operand:", c_color_expr);
    print_location(n.loc);
    std::cout << '\n';
}*/

void text_ast_dumper::dump_txt_literal(txt_literal const& n)
{
    std::cout << indent(depth);
    write_color("text_literal ", c_color_expr);
    print_location(n.loc);
    std::cout << ' ';
    if (n.is_character)
    {
        write_color("(char) ", c_color_expr);
    }
    else
    {
        write_color("(string) ", c_color_expr);
    }
    std::cout << '"';
    std::cout << n.value;
    std::cout << "\";\n";
    if (show_expr_types)
    {
        ++depth;
        dump_expr_type(n.type, n.cat);
        --depth;
    }
}

void text_ast_dumper::dump_num_literal(num_literal const& n)
{
    std::cout << indent(depth);
    write_color("number_literal ", c_color_expr);
    print_location(n.loc);
    std::cout << ' ';
    std::cout << n.value;
    std::cout << ";\n";
    if (show_expr_types)
    {
        ++depth;
        dump_expr_type(n.type, n.cat);
        --depth;
    }
}

void text_ast_dumper::dump_cast_expr(cast_expr const& n)
{
    std::cout << indent(depth);
    write_color("cast_expr ", c_color_expr);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    // Exception: always print type of a cast expression
    dump_expr_type(n.type, n.cat);
    dump_expr(*n.operand);
    --depth;
}

// Declarations

void text_ast_dumper::dump_var_decl(var_decl const& n)
{
    std::cout << indent(depth);
    write_color("variable_declaration ", c_color_decl);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    dump_basic_type(n.type);
    dump_identifier(n.ident);
    if (n.init)
        dump_expr(*n.init);
    --depth;
}

void text_ast_dumper::dump_entry_decl(entry_decl const& n)
{
    std::cout << indent(depth);
    write_color("entry_declaration ", c_color_decl);
    print_location(n.loc);
    std::cout << ' ';
    std::cout << "(no_args)";
    std::cout << ":\n";
    ++depth;
    dump_stmt(*n.code);
    --depth;
}

void text_ast_dumper::dump_import_decl(import_decl const& n)
{
    std::cout << indent(depth);
    write_color("import_declaration ", c_color_decl);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    std::cout << indent(depth) << "filename: \"";
    write_color(n.filename, c_color_identifier);
    std::cout << "\"\n";
    --depth;
}

void text_ast_dumper::dump_extern_decl(extern_decl const& n)
{
    std::cout << indent(depth);
    write_color("extern ", c_color_decl);
    print_location(n.loc);
    std::cout << '\n';

    ++depth;
    std::cout << indent(depth) << "real name: \"";
    write_color(n.real_name, c_color_identifier);
    std::cout << "\"\n";
    dump_decl(*n.inner_decl);
    --depth;
}

void text_ast_dumper::dump_parameters(parameters const& n)
{
    std::cout << indent(depth);
    write_color("parameters ", c_color_decl);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    for (auto& param : n.body)
    {
        dump_var_decl(*param);
    }
    --depth;
}

void text_ast_dumper::dump_func_decl(func_decl const& n)
{
    std::cout << indent(depth);
    write_color("function_declaration ", c_color_decl);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    dump_basic_type(n.result_type);
    dump_identifier(n.ident);
    dump_parameters(n.params);
    --depth;
}

void text_ast_dumper::dump_func_def(func_def const& n)
{
    std::cout << indent(depth);
    write_color("function_definition ", c_color_decl);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    dump_basic_type(n.result_type);
    dump_identifier(n.ident);
    dump_parameters(n.params);
    dump_stmt(*n.body);
    --depth;
}

// Statements

void text_ast_dumper::dump_decl_stmt(decl_stmt const& n)
{
    std::cout << indent(depth);
    write_color("declaration_statement ", c_color_stmt);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    dump_decl(*n.inner_decl);
    --depth;
}

void text_ast_dumper::dump_assign_stmt(assign_stmt const& n)
{
    std::cout << indent(depth);
    write_color("assign_statement ", c_color_stmt);
    print_location(n.loc);
    std::cout << " (";
    std::cout << exprop_id(n.assign_op);
    std::cout << ' ';
    print_location(n.assign_loc);
    std::cout << ")\n";
    ++depth;
    dump_expr(*n.target);
    dump_expr(*n.value);
    --depth;
}

void text_ast_dumper::dump_compound_stmt(compound_stmt const& n)
{
    std::cout << indent(depth);
    write_color("compound_statement ", c_color_stmt);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    for (auto& s : n.body)
    {
        dump_stmt(*s);
    }
    --depth;
}

void text_ast_dumper::dump_ret_stmt(ret_stmt const& n)
{
    std::cout << indent(depth);
    write_color("ret_statement ", c_color_stmt);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    dump_expr(*n.val);
    --depth;
}

void text_ast_dumper::dump_conditional_stmt(conditional_stmt const& n)
{
    std::cout << indent(depth);
    write_color("conditional_statement ", c_color_stmt);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    dump_expr(*n.cond);
    dump_stmt(*n.true_branch);
    if (n.false_branch)
        dump_stmt(*n.false_branch);
    --depth;
}

void text_ast_dumper::dump_iteration_stmt(iteration_stmt const& n)
{
    std::cout << indent(depth);
    write_color("iteration_statement ", c_color_stmt);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    dump_expr(*n.cond);
    dump_stmt(*n.loop_body);
    --depth;
}

void text_ast_dumper::dump_expr_stmt(expr_stmt const& n)
{
    std::cout << indent(depth);
    write_color("expression_statement ", c_color_stmt);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    dump_expr(*n.node);
    --depth;
}

void text_ast_dumper::dump_null_stmt(stmt const& n)
{
    std::cout << indent(depth);
    write_color("null_statement ", c_color_stmt);
    print_location(n.loc);
    std::cout << '\n';
}
