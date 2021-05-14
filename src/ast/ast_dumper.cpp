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
constexpr color c_color_expr = color::blue | color::bright | color::bold;
constexpr color c_color_decl = color::green | color::bright | color::bold;
constexpr color c_color_stmt = color::red | color::blue | color::bright | color::bold;
constexpr color c_color_identifier = color::blue | color::bright;
constexpr color c_color_location = color::red | color::green;

std::string indent(int x)
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
std::string dump_dtname(dtypename n)
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

std::string dump_dtmod(dtypemod m)
{
    std::string result;
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

std::string exprop_id(exprop op)
{
    std::string result;
    result = "(";
    switch (op)
    {
    case static_cast<exprop>('+'):
        result += "add";
        break;
    case static_cast<exprop>('-'):
        result += "sub";
        break;
    case static_cast<exprop>('/'):
        result += "div";
        break;
    case static_cast<exprop>('*'):
        result += "mult";
        break;
    case exprop::deref:
        result += "deref";
        break;
    case exprop::ref:
        result += "ref";
        break;
    case exprop::as:
        result += "as";
        break;
    case exprop::call:
        result += "call";
        break;
    case exprop::none:
    default:
        result += "invalid";
    }
    result += ")";
    return result;
}

// === AST UTIL DUMPS ===

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

// Dispatch functions

void text_ast_dumper::dump_expr(expr const& node)
{
    switch (node.kind)
    {
        case optype::lit:
            if (static_cast<literal_node const&>(node).ltype == littype::txt)
                return dump_txt_literal(static_cast<txt_literal const&>(node));
            return dump_num_literal(static_cast<num_literal const&>(node));
        case optype::ident:
            return dump_identifier(static_cast<identifier const&>(node));
        case optype::call:
            return dump_func_call(static_cast<func_call const&>(node));
        case optype::op:
            return dump_binop(static_cast<binop const&>(node));
        case optype::invalid:
        default:
            return;
    }
}

void text_ast_dumper::dump_decl(decl const& node)
{
    switch (node.type)
    {
        case decl_type::var:
            return dump_var_decl(static_cast<var_decl const&>(node));
        case decl_type::entry:
            return dump_entry_decl(static_cast<entry_decl const&>(node));
        case decl_type::import:
            return dump_import_decl(static_cast<import_decl const&>(node));
        case decl_type::fdecl:
            return dump_func_decl(static_cast<func_decl const&>(node));
        case decl_type::fdef:
            return dump_func_def(static_cast<func_def const&>(node));
        case decl_type::external:
            return dump_extern_decl(static_cast<extern_decl const&>(node));
        default:
            return;
    }
}

void text_ast_dumper::dump_stmt(stmt const& node)
{
    switch (node.type)
    {
        case stmt_type::decl:
            return dump_decl_stmt(static_cast<decl_stmt const&>(node));
        case stmt_type::assign:
            return dump_assign_stmt(static_cast<assign_stmt const&>(node));
        case stmt_type::compound:
            return dump_compound_stmt(static_cast<compound_stmt const&>(node));
        case stmt_type::ret:
            return dump_ret_stmt(static_cast<ret_stmt const&>(node));
        case stmt_type::conditional:
            return dump_conditional_stmt(static_cast<conditional_stmt const&>(node));
        case stmt_type::iteration:
            return dump_iteration_stmt(static_cast<iteration_stmt const&>(node));
        case stmt_type::expr:
            return dump_expr_stmt(static_cast<expr_stmt const&>(node));
        default:
            return;
    }
}



void text_ast_dumper::dump_exprtype(exprtype const& t)
{
    std::cout << indent(depth);
    std::cout << "data_type\n";
    std::cout << indent(depth + 1);
    std::cout << "typename ";
    write_color(dump_dtname(t.basetp), c_color_type);
    std::cout << ";\n";
    std::cout << indent(depth + 1);
    if (t.exttp.empty())
    {
        std::cout << "(no type modifiers)\n";
    }
    else
    {
        std::cout << "type modifiers:\n";
        // I really gave up on naming thing well there
        for (std::byte x : t.exttp)
        {
            dtypemod w = static_cast<dtypemod>(x);
            std::cout << indent(depth + 2);
            std::cout << dump_dtmod(w);
            std::cout << '\n';
        }
    }
}

void text_ast_dumper::dump_binop(binop const& n)
{
    std::cout << indent(depth);
    write_color("binop ", c_color_expr);
    print_location(n.loc);
    std::cout << ' ';
    std::cout << exprop_id(n.op);
    std::cout << ' ';
    print_location(n.op_loc);
    std::cout << '\n';
    ++depth;
    std::cout << indent(depth);
    std::cout << "left:\n";
    dump_expr(*n.left);
    std::cout << indent(depth);
    std::cout << "right:\n";
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
    dump_expr(*n.callee);
    dump_arguments(n.args);
    --depth;
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
}

void text_ast_dumper::dump_num_literal(num_literal const& n)
{
    std::cout << indent(depth);
    write_color("number_literal ", c_color_expr);
    print_location(n.loc);
    std::cout << ' ';
    std::cout << n.value;
    std::cout << ";\n";
}


void text_ast_dumper::dump_var_decl(var_decl const& n)
{
    std::cout << indent(depth);
    write_color("variable_declaration ", c_color_decl);
    print_location(n.loc);
    std::cout << '\n';
    ++depth;
    dump_exprtype(n.var_type);
    dump_identifier(*n.ident);
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
    dump_txt_literal(n.filename);
    --depth;
}

void text_ast_dumper::dump_extern_decl(extern_decl const& n)
{
    std::cout << indent(depth);
    write_color("extern ", c_color_decl);
    print_location(n.loc);
    std::cout << '\n';

    ++depth;
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
    dump_exprtype(n.data_type);
    dump_identifier(*n.ident);
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
    dump_exprtype(n.data_type);
    dump_identifier(*n.ident);
    dump_parameters(n.params);
    dump_stmt(*n.body);
    --depth;
}


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
    std::cout << '\n';
    ++depth;
    dump_identifier(*n.ident);
    // TODO: Print variable after analysis
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
