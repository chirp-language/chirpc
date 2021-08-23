#pragma once
#include "../seman/tracker.hpp"
#include "../ast/ast.hpp"
#include "../shared/diagnostic.hpp"
#include <string>
#include <vector>

class codegen
{
    public:
    void gen();

    void set_tree(ast_root*, std::string);

    // The name is pretty bad ngl
    std::string&& get_result();

    bool errored = false;

    codegen(diagnostic_manager& diag)
        : diagnostics(diag) {}
    private:
    diagnostic_manager& diagnostics;

    std::string emit_identifier(identifier const&);
    std::string emit_raw_qual_identifier(raw_qual_identifier const&);
    std::string emit_decl_symbol_name(decl const*);
    std::string emit_datatype(basic_type const&);
    std::string emit_expr(expr const&);
    std::string emit_binop(binop const&);
    std::string emit_unop(unop const&);
    std::string emit_arguments(arguments const&);
    std::string emit_func_call(func_call const&);
    std::string emit_id_ref_expr(id_ref_expr const&);
    std::string emit_string_literal(string_literal const&);
    std::string emit_integral_literal(integral_literal const&);
    std::string emit_nullptr_literal(nullptr_literal const&);
    std::string emit_cast_expr(cast_expr const&);

    std::string emit_decl(decl const&);
    std::string emit_var_decl(var_decl const&);
    std::string emit_entry_decl(entry_decl const&);
    std::string emit_import_decl(import_decl const&);
    std::string emit_namespace_decl(namespace_decl const&);
    std::string emit_parameters(parameters const&);
    std::string emit_func_decl(func_decl const&);
    std::string emit_func_def(func_def const&);
    std::string emit_extern_decl(extern_decl const&);

    std::string emit_stmt(stmt const&);
    std::string emit_assign_stmt(assign_stmt const&);
    std::string emit_compound_stmt(compound_stmt const&);
    std::string emit_ret_stmt(ret_stmt const&);
    std::string emit_conditional_stmt(conditional_stmt const&);
    std::string emit_iteration_stmt(iteration_stmt const&);

    void gen_toplevel();

    ast_root* m_tree;
    std::string result;
    std::string filename;
};
