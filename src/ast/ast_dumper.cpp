#include "ast_dumper.hpp"

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
constexpr color c_color_type_error = color::red | color::bright | color::bold;
constexpr color c_color_expr = color::blue | color::bright | color::bold;
constexpr color c_color_decl = color::green | color::bright | color::bold;
constexpr color c_color_stmt = color::red | color::blue | color::bright | color::bold;
constexpr color c_color_identifier = color::blue | color::bright;
constexpr color c_color_location = color::red | color::green;

void text_dumper_base::indent(int depth)
{
    for (int i = 0; i < depth * 3; ++i)
        std::cout << ' ';
}

void text_ast_dumper::print_location(location_range loc) {
    if (loc_prov)
        write_color(loc_prov->print_loc(loc), c_color_location);
    else
        write_color("<unavail>", c_color_location);
}

// I don't even care about names now
static char const* dump_dtname(dtypename n)
{
    switch (n)
    {
    case dtypename::_int:
        return "int";
    case dtypename::_long:
        return "long";
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
    }
    chirp_unreachable("dump_dtname");
}

static char const* dump_dtmod(dtypemod m)
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
    }
    chirp_unreachable("dump_dtmod");
}

static char const* dump_cast_kind(cast_kind k) {
    switch (k)
    {
    case cast_kind::_invalid:
        return "invalid";
    case cast_kind::_explicit:
        return "explicit_cast";
    case cast_kind::_const:
        return "const_cast";
    case cast_kind::_grade:
        return "grade_cast";
    case cast_kind::_sign:
        return "sign_cast";
    case cast_kind::_cat:
        return "cat_cast";
    case cast_kind::_float:
        return "float_cast";
    case cast_kind::_bool:
        return "bool_cast";
    }
    chirp_unreachable("dump_cast_kind");
}

static char const* dump_exprcat(exprcat c)
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
    chirp_unreachable("dump_exprcat");
}

char const* exprop_id(tkn_type op)
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
    case tkn_type::kw_alloca:
        return "alloca";
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

    #if 0
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

    if(root.nspaces.empty())
    {
        write_color("-- No namespaces --\n", c_color_top_level_unavail);
    }
    else
    {
        for(auto& node : root.nspaces)
        {
            dump_namespace_decl(*node);
        }
    }

    if (root.fdecls.empty())
    {
        write_color("-- No function declarations --\n", c_color_top_level_unavail);
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
        write_color("-- No function definitions --\n", c_color_top_level_unavail);
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
    #endif
    for (auto& node : root.top_decls)
    {
        dump_decl(*node);
    }
    if (root.top_decls.empty())
    {
        write_color("-- No declarations --\n", c_color_top_level_unavail);
    }

    if (root.entry)
    {
        write_color("-- Entry available --\n", c_color_top_level);
    }
    else {
        write_color("-- No entry --\n", c_color_top_level_unavail);
    }
}

void text_ast_dumper::dump_identifier(identifier const& n)
{
    indent(depth);
    write_color("identifier ", c_color_identifier);
    print_location(n.loc);
    std::cout << ' ' << n.name;
    std::cout << '\n';
}

void text_ast_dumper::dump_qual_identifier(qual_identifier const& n)
{
    indent(depth);
    write_color("qual_identifier ", c_color_identifier);
    print_location(n.loc);
    std::cout << ' ';
    int i = 0, len = n.parts.size();
    for (auto const& id : n.parts)
    {
        std::cout << id.name;
        if (++i != len)
        {
            std::cout << ".";
        }
    }
    std::cout << '\n';
}

// Dispatch functions

void text_ast_dumper::dump_expr(expr const& node)
{
    switch (node.kind)
    {
        case expr_kind::binop:
            return dump_binop(static_cast<binop const&>(node));
        case expr_kind::unop:
            return dump_unop(static_cast<unop const&>(node));
        case expr_kind::call:
            return dump_func_call(static_cast<func_call const&>(node));
        case expr_kind::ident:
            return dump_id_ref_expr(static_cast<id_ref_expr const&>(node));
        case expr_kind::strlit:
            return dump_string_literal(static_cast<string_literal const&>(node));
        case expr_kind::intlit:
            return dump_integral_literal(static_cast<integral_literal const&>(node));
        case expr_kind::nulllit:
            return dump_nullptr_literal(static_cast<nullptr_literal const&>(node));
        case expr_kind::cast:
            return dump_cast_expr(static_cast<cast_expr const&>(node));
    }
}

void text_ast_dumper::dump_decl(decl const& node)
{
    switch (node.kind)
    {
        case decl_kind::root:
            return dump_ast(static_cast<ast_root const&>(node));
        case decl_kind::var:
            return dump_var_decl(static_cast<var_decl const&>(node));
        case decl_kind::entry:
            return dump_entry_decl(static_cast<entry_decl const&>(node));
        case decl_kind::import:
            return dump_import_decl(static_cast<import_decl const&>(node));
        case decl_kind::nspace:
            return dump_namespace_decl(static_cast<namespace_decl const&>(node));
        case decl_kind::fdecl:
            return dump_func_decl(static_cast<func_decl const&>(node));
        case decl_kind::fdef:
            return dump_func_def(static_cast<func_def const&>(node));
        case decl_kind::external:
            return dump_extern_decl(static_cast<extern_decl const&>(node));
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
    }
}

// Expressions

void text_ast_dumper::dump_basic_type(basic_type const& type)
{
    indent(depth);
    std::cout << "basic_type ";
    // Remember that order is reversed
    for (auto it = type.exttp.rbegin(), end = type.exttp.rend(); it != end; ++it)
    {
        write_color(dump_dtmod(*it), c_color_type);
        std::cout << ' ';
    }
    write_color(dump_dtname(type.basetp), c_color_type);
    std::cout << ";\n";
}

void text_ast_dumper::dump_expr_type(basic_type const& type, exprcat cat)
{
    indent(depth);
    std::cout << "expr_type ";
    if (cat == exprcat::error or cat == exprcat::unset)
        begin_color(c_color_type_error);
    else
        begin_color(c_color_type_cat);
    std::cout << dump_exprcat(cat);
    end_color();
    std::cout << ' ';
    // Remember that order is reversed
    for (auto it = type.exttp.rbegin(), end = type.exttp.rend(); it != end; ++it)
    {
        write_color(dump_dtmod(*it), c_color_type);
        std::cout << ' ';
    }
    write_color(dump_dtname(type.basetp), c_color_type);
    std::cout << ";\n";
}

void text_ast_dumper::dump_binop(binop const& n)
{
    indent(depth);
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

void text_ast_dumper::dump_unop(unop const& n)
{
    indent(depth);
    write_color("unop ", c_color_expr);
    print_location(n.loc);
    std::cout << " (";
    std::cout << exprop_id(n.op);
    std::cout << ' ';
    print_location(n.op_loc);
    std::cout << ")\n";
    ++depth;
    if (show_expr_types)
        dump_expr_type(n.type, n.cat);
    dump_expr(*n.operand);
    --depth;
}

void text_ast_dumper::dump_arguments(arguments const& n)
{
    indent(depth);
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
    indent(depth);
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
    indent(depth);
    write_color("id_ref_expr ", c_color_expr);
    print_location(n.loc);
    std::cout << ' ';
    int i = 0;
    for (auto const& id : n.ident.parts)
    {
        std::cout << id.name;
        if (++i != n.ident.parts.size())
        {
            std::cout << ".";
        }
    }
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
    indent(depth);
    write_color("left_operand:", c_color_expr);
    print_location(n.loc);
    std::cout << '\n';
}*/

void text_ast_dumper::dump_string_literal(string_literal const& n)
{
    indent(depth);
    write_color("string_literal ", c_color_expr);
    print_location(n.loc);
    std::cout << " \"";
    // Escape it?
    std::cout << n.value;
    std::cout << "\"\n";
    if (show_expr_types)
    {
        ++depth;
        dump_expr_type(n.type, n.cat);
        --depth;
    }
}

void text_ast_dumper::dump_integral_literal(integral_literal const& n)
{
    indent(depth);
    write_color("integral_literal ", c_color_expr);
    print_location(n.loc);
    std::cout << ' ';
    write_color(std::to_string(n.value.val), c_color_expr);
    std::cout << '\n';
    ++depth;
    dump_expr_type(n.type, n.cat);
    --depth;
}

void text_ast_dumper::dump_nullptr_literal(nullptr_literal const& n)
{
    indent(depth);
    write_color("nullptr_literal ", c_color_expr);
    print_location(n.loc);
    std::cout << '\n';
    if (show_expr_types)
    {
        ++depth;
        dump_expr_type(n.type, n.cat);
        --depth;
    }
}

void text_ast_dumper::dump_cast_expr(cast_expr const& n)
{
    indent(depth);
    write_color("cast_expr ", c_color_expr);
    print_location(n.loc);
    std::cout << ' ';
    if (n.ckind == cast_kind::_invalid)
        begin_color(c_color_type_error);
    else
        begin_color(c_color_type_cat);
    std::cout << dump_cast_kind(n.ckind);
    end_color();
    std::cout << '\n';
    ++depth;
    // Exception: always print type of a cast expression, if it's an explicit cast
    if (show_expr_types or n.ckind == cast_kind::_explicit)
        dump_expr_type(n.type, n.cat);
    dump_expr(*n.operand);
    --depth;
}

// Declarations

void text_ast_dumper::dump_var_decl(var_decl const& n)
{
    indent(depth);
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
    indent(depth);
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
    indent(depth);
    write_color("import_declaration ", c_color_decl);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    indent(depth);
    std::cout << "filename: \"";
    write_color(n.filename, c_color_identifier);
    std::cout << "\"\n";
    --depth;
}

void text_ast_dumper::dump_extern_decl(extern_decl const& n)
{
    indent(depth);
    write_color("extern ", c_color_decl);
    print_location(n.loc);
    std::cout << '\n';

    ++depth;
    indent(depth);
    std::cout << "real name: \"";
    write_color(n.real_name, c_color_identifier);
    std::cout << "\"\n";
    dump_decl(*n.inner_decl);
    --depth;
}

void text_ast_dumper::dump_namespace_decl(namespace_decl const& n)
{
    indent(depth);
    write_color("namespace_declaration ", c_color_decl);
    print_location(n.loc);
    std::cout << '\n';

    ++depth;
    dump_identifier(n.ident);
    for(auto& node : n.decls)
    {
        dump_decl(*node);
    }
    --depth;
}

void text_ast_dumper::dump_parameters(parameters const& n)
{
    indent(depth);
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
    indent(depth);
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
    indent(depth);
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
    indent(depth);
    write_color("declaration_statement ", c_color_stmt);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    dump_decl(*n.inner_decl);
    --depth;
}

void text_ast_dumper::dump_assign_stmt(assign_stmt const& n)
{
    indent(depth);
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
    indent(depth);
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
    indent(depth);
    write_color("ret_statement ", c_color_stmt);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    dump_expr(*n.val);
    --depth;
}

void text_ast_dumper::dump_conditional_stmt(conditional_stmt const& n)
{
    indent(depth);
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
    indent(depth);
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
    indent(depth);
    write_color("expression_statement ", c_color_stmt);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    dump_expr(*n.node);
    --depth;
}

void text_ast_dumper::dump_null_stmt(stmt const& n)
{
    indent(depth);
    write_color("null_statement ", c_color_stmt);
    print_location(n.loc);
    std::cout << '\n';
}
