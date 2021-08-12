/// \file Semantic analyser and AST walker

#pragma once

#include "../ast/ast.hpp"
#include "tracker.hpp"

class analyser
{
	public:
	analyser(ast_root& root, diagnostic_manager& diag)
		: root(root), sym_tracker(diag), diagnostics(diag)
	{
		sym_tracker.set_root(&root);
	}

	void analyse();

	protected:
	ast_root& root;
	tracker sym_tracker;
	diagnostic_manager& diagnostics;

	void visit_expr(expr& node);
	void visit_decl(decl& node);
	void visit_stmt(stmt& node);

	// Expressions
	void visit_basic_type(basic_type&) = delete;
	void visit_binop(binop&);
	void visit_unop(unop&);
	void visit_arguments(arguments&);
	void visit_func_call(func_call&);
	void visit_id_ref_expr(id_ref_expr&);
	void visit_loperand(loperand&) = delete;
	void visit_string_literal(string_literal&);
	void visit_integral_literal(integral_literal&);
	void visit_nullptr_literal(nullptr_literal&);
	void visit_cast_expr(cast_expr&);
	// Declarations
	void visit_var_decl(var_decl&);
	void visit_entry_decl(entry_decl&);
	void visit_import_decl(import_decl&);
	void visit_extern_decl(extern_decl&);
	void visit_namespace_decl(namespace_decl&);
	void visit_parameters(parameters&);
	void visit_func_decl(func_decl&);
	void visit_func_def(func_def&);
	// Statements
	void visit_decl_stmt(decl_stmt&);
	void visit_assign_stmt(assign_stmt&);
	void visit_compound_stmt(compound_stmt&);
	void visit_ret_stmt(ret_stmt&);
	void visit_conditional_stmt(conditional_stmt&);
	void visit_iteration_stmt(iteration_stmt&);
	void visit_expr_stmt(expr_stmt&);
	void visit_null_stmt(stmt&); // null_stmt contains no further members
};
