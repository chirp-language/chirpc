#include "analyser.hpp"
#include "tracker.hpp"
#include <cstdlib>

// Dispatch functions

void analyser::analyse()
{
	#if 0
	for (auto& d : root.imports)
		visit_import_decl(*d);
	for (auto& d : root.externs)
		visit_extern_decl(*d);
	for (auto& d : root.nspaces)
		visit_namespace_decl(*d);
	for (auto& d : root.fdecls)
		visit_func_decl(*d);
	for (auto& d : root.fdefs)
		visit_func_def(*d);
	#endif
	// Top scope (already pushed)
	for (auto& d : root.top_decls)
		visit_decl(*d);
}

void analyser::visit_expr(expr& node)
{
	switch (node.kind)
	{
		case expr_kind::binop:
			return visit_binop(static_cast<binop&>(node));
		case expr_kind::unop:
			return visit_unop(static_cast<unop&>(node));
		case expr_kind::call:
			return visit_func_call(static_cast<func_call&>(node));
		case expr_kind::ident:
			return visit_id_ref_expr(static_cast<id_ref_expr&>(node));
		case expr_kind::strlit:
			return visit_string_literal(static_cast<string_literal&>(node));
		case expr_kind::intlit:
			return visit_integral_literal(static_cast<integral_literal&>(node));
		case expr_kind::nulllit:
			return visit_nullptr_literal(static_cast<nullptr_literal&>(node));
		case expr_kind::cast:
			return visit_cast_expr(static_cast<cast_expr&>(node));
	}
}

void analyser::visit_decl(decl& node)
{
	switch (node.kind)
	{
		case decl_kind::root:
			// Shouldn't happen
			break;
		case decl_kind::var:
			return visit_var_decl(static_cast<var_decl&>(node));
		case decl_kind::entry:
			return visit_entry_decl(static_cast<entry_decl&>(node));
		case decl_kind::import:
			return visit_import_decl(static_cast<import_decl&>(node));
		case decl_kind::nspace:
			return visit_namespace_decl(static_cast<namespace_decl&>(node));
		case decl_kind::fdecl:
			return visit_func_decl(static_cast<func_decl&>(node));
		case decl_kind::fdef:
			return visit_func_def(static_cast<func_def&>(node));
		case decl_kind::external:
			return visit_extern_decl(static_cast<extern_decl&>(node));
	}
}

void analyser::visit_stmt(stmt& node)
{
	switch (node.kind)
	{
		case stmt_kind::decl:
			return visit_decl_stmt(static_cast<decl_stmt&>(node));
		case stmt_kind::assign:
			return visit_assign_stmt(static_cast<assign_stmt&>(node));
		case stmt_kind::compound:
			return visit_compound_stmt(static_cast<compound_stmt&>(node));
		case stmt_kind::ret:
			return visit_ret_stmt(static_cast<ret_stmt&>(node));
		case stmt_kind::conditional:
			return visit_conditional_stmt(static_cast<conditional_stmt&>(node));
		case stmt_kind::iteration:
			return visit_iteration_stmt(static_cast<iteration_stmt&>(node));
		case stmt_kind::expr:
			return visit_expr_stmt(static_cast<expr_stmt&>(node));
		case stmt_kind::null:
			return visit_null_stmt(node);
	}
}

// Expressions

void analyser::visit_binop(binop& node)
{
	visit_expr(*node.left);
	visit_expr(*node.right);

	if (node.left->cat == exprcat::error or node.right->cat == exprcat::error)
	{
		node.cat = exprcat::error;
	}
	else if (node.left->type != node.right->type)
	{
		node.cat = exprcat::error;

		diagnostic(!soft_type_checks ? diagnostic_type::location_err : diagnostic_type::location_warning)
			.at(node.op_loc)
			.reason("Operand types don't match")
			.report(diagnostics);
	}
	else
	{
		// TODO: Do deeper type analysis
		node.cat = exprcat::rval;
		if (node.op >= tkn_type::cmp_S
			and node.op <= tkn_type::cmd_E)
		{
			node.type.basetp = dtypename::_bool;
		}
		else
		{
			node.type = node.left->type;
		}
	}
}

void analyser::visit_unop(unop& node)
{
	visit_expr(*node.operand);

	if (node.operand->cat == exprcat::error)
	{
		node.cat = exprcat::error;
		return;
	}

	switch (node.op)
	{
		case tkn_type::plus_op:
			node.cat = exprcat::rval;
			node.type = node.operand->type;
			// Do promotion
			break;
		case tkn_type::minus_op:
			node.cat = exprcat::rval;
			node.type = node.operand->type;
			// TODO: Assert type is an integer, do promotion
			break;
		case tkn_type::ref_op:
			if (node.operand->cat != exprcat::lval)
			{
				diagnostic(!soft_type_checks ? diagnostic_type::location_err : diagnostic_type::location_warning)
					.at(node.loc)
					.reason("Cannot take address of a non-lvalue")
					.report(diagnostics);
				node.cat = exprcat::error;
				return;
			}
			node.cat = exprcat::rval;
			node.type = node.operand->type;
			node.type.exttp.push_back(static_cast<std::byte>(dtypemod::_ptr));
			break;
		case tkn_type::deref_op:
			if (node.operand->type.exttp.empty() or node.operand->type.exttp.back() != static_cast<std::byte>(dtypemod::_ptr))
			{
				diagnostic(!soft_type_checks ? diagnostic_type::location_err : diagnostic_type::location_warning)
					.at(node.loc)
					.reason("Cannot dereference a non-pointer")
					.report(diagnostics);
				node.cat = exprcat::error;
				return;
			}
			node.cat = exprcat::lval;
			node.type = node.operand->type;
			node.type.exttp.pop_back();
			break;
		case tkn_type::kw_alloca:
			// Type checking couldn't literally get more shitty lol, but not now
			if (node.operand->type.basetp != dtypename::_long
			    or !node.operand->type.exttp.empty())
			{
				diagnostic(!soft_type_checks ? diagnostic_type::location_err : diagnostic_type::location_warning)
					.at(node.loc)
					.reason("Alloca takes an integer argument")
					.report(diagnostics);
				node.cat = exprcat::error;
				return;
			}
			node.cat = exprcat::rval;
			node.type.basetp = dtypename::_none;
			node.type.exttp.push_back(static_cast<std::byte>(dtypemod::_ptr));
			break;
		default:
			std::abort();
	}
}

void analyser::visit_arguments(arguments& node)
{
	for (auto& e : node.body)
	{
		visit_expr(*e);
	}
}

void analyser::visit_func_call(func_call& node)
{
	visit_expr(*node.callee);
	visit_arguments(node.args);

	node.type = node.callee->type;
	if (!node.type.exttp.empty() and static_cast<dtypemod>(node.type.exttp.back()) == dtypemod::_func)
	{
		node.type.exttp.pop_back();
		node.cat = exprcat::rval;
	}
	else
	{
		node.cat = exprcat::error;
		diagnostic(!soft_type_checks ? diagnostic_type::location_err : diagnostic_type::location_warning)
			.at(node.loc)
			.reason("Cannot call non-function")
			.report(diagnostics);
	}
}

void analyser::visit_id_ref_expr(id_ref_expr& node)
{
	//! Point of interest
	if (auto* sym = sym_tracker.lookup_sym_qual(node.ident))
	{
		auto const* decl = sym->target;
		while (decl->kind == decl_kind::external)
			decl = static_cast<extern_decl const*>(decl)->inner_decl.get();
		node.target = decl;
		switch (decl->kind)
		{
			case decl_kind::var:
				node.type = static_cast<var_decl const&>(*decl).type;
				node.cat = exprcat::lval;
				break;
			case decl_kind::fdecl:
			case decl_kind::fdef:
				node.type = static_cast<func_decl const&>(*decl).result_type;
				node.cat = exprcat::rval;
				node.type.exttp.push_back(static_cast<std::byte>(dtypemod::_func));
				break;
			default:
				node.cat = exprcat::error;
				diagnostic(!soft_type_checks ? diagnostic_type::location_err : diagnostic_type::location_warning)
					.at(node.loc)
					.reason("Unknown declaration type")
					.report(diagnostics);
		}
	}
	else
	{
		node.cat = exprcat::error;
	}
}

void analyser::visit_string_literal(string_literal& node)
{
	if (node.cat != exprcat::unset)
		return;
	node.cat = exprcat::rval;
	node.type.basetp = dtypename::_char;
	node.type.exttp.push_back(static_cast<std::byte>(dtypemod::_const));
	node.type.exttp.push_back(static_cast<std::byte>(dtypemod::_ptr));
}

void analyser::visit_integral_literal(integral_literal& node)
{
	// Category & type should've been set in the parser
	if (node.cat == exprcat::unset)
		node.cat = exprcat::error;
}

void analyser::visit_nullptr_literal(nullptr_literal& node)
{
	// Category & type already set in parser
	// No need to do anything
}

void analyser::visit_cast_expr(cast_expr& node)
{
	// Category & type already set
	visit_expr(*node.operand);
}

// Declarations

void analyser::visit_var_decl(var_decl& node)
{
	//! Point of interest
	auto* sym = sym_tracker.decl_sym(node.ident, node);
	// Inherit from enclosing scope (parameter scope is private)
	sym->is_global = sym_tracker.get_scope()->is_global;
	sym->has_storage = true;
	if (!sym_tracker.bind_sym(sym))
	{
		diagnostic(diagnostic_type::location_err)
			.at(node.loc)
			.reason("A variable with the same name already exists")
			.report(this->diagnostics);
		return;
	}

	// Check for invalid type
	if (node.type.basetp == dtypename::_none)
	{
		// Checks if it's a pointer.
		bool is_ptr = false;
		for (std::byte d : node.type.exttp)
		{
			if (static_cast<dtypemod>(d) == dtypemod::_ptr)
			{
				is_ptr = true;
				break;
			}
		}

		if (!is_ptr)
		{
			diagnostic(diagnostic_type::location_err)
				.at(node.loc)
				.reason("Variable cannot be of type `none`, unless it's a pointer")
				.report(this->diagnostics);
		}
	}
	// TODO: Convert from initializer type (if any) to variable type
	if (node.init)
		visit_expr(*node.init);
}

void analyser::visit_entry_decl(entry_decl& node)
{
	auto* sym = sym_tracker.decl_sym();
	sym->has_storage = true;
	sym->is_entry = true;
	sym->target = &node;
	node.symbol = sym;
	visit_compound_stmt(static_cast<compound_stmt&>(*node.code));
}

void analyser::visit_import_decl(import_decl& node)
{
	// TODO: Bind name
}

void analyser::visit_extern_decl(extern_decl& node)
{
	visit_decl(*node.inner_decl);
	if (auto* sym = node.inner_decl->symbol)
	{
		node.symbol = sym;
		sym->full_name.parts.clear();
		sym->full_name.parts.push_back(identifier::from(std::string(node.real_name), node.name_loc));
	}
	else
	{
		diagnostic(diagnostic_type::location_err)
			.at(node.loc)
			.reason("Extern declaration doesn't declare a symbol")
			.report(diagnostics);
	}
}

void analyser::visit_namespace_decl(namespace_decl& node)
{
	// This is even more hacky beyond any belief (pt. 1)
	auto* sym = sym_tracker.decl_sym(node.ident, node);
	sym->is_global = true;
	sym->has_storage = false;
	if (!sym_tracker.bind_sym(sym))
		return;
	sym_tracker.push_scope(sym);
	for (auto& d : node.decls)
	{
		// Push namespace context
		visit_decl(*d);
	}
	sym_tracker.pop_scope();
}

void analyser::visit_parameters(parameters& node)
{
	for (auto& i : node.body)
	{
		visit_var_decl(*i);
	}
}

void analyser::visit_func_decl(func_decl& node)
{
	//! Point of interest

	auto* sym = sym_tracker.decl_sym(node.ident, node);
	sym->is_global = true;
	sym->has_storage = false;
	if (!sym_tracker.bind_sym(sym))
	{
		// TODO: Check if declarations match
		diagnostic(diagnostic_type::location_err)
			.at(node.loc)
			.reason("A symbol with that name already exists")
			.report(diagnostics);
	}

	auto* body_scope = sym_tracker.decl_sym();
	body_scope->is_global = false;
	sym_tracker.push_scope(body_scope);
	visit_parameters(node.params);
	sym_tracker.pop_scope();
}

void analyser::visit_func_def(func_def& node)
{
	//! Point of interest

	tracker::symbol* sym;
	if ((sym = sym_tracker.find_sym_cur(node.ident)))
	{
		// TODO: Check if declarations match
		if (sym->target->kind == decl_kind::fdecl)
		{
			// Rebind the symbol to point to definition
			sym->target = &node;
			node.symbol = sym;
		}
		else
		{
			diagnostic(diagnostic_type::location_err)
				.at(node.loc)
				.reason("A symbol with that name already exists")
				.report(diagnostics);
		}
	}
	else
	{
		sym = sym_tracker.decl_sym(node.ident, node);
		sym->is_global = true;
		sym->has_storage = true;
		sym_tracker.push_sym_unsafe(sym);
	}

	auto* body_scope = sym_tracker.decl_sym();
	body_scope->is_global = false;
	sym_tracker.push_scope(body_scope);
	visit_parameters(node.params);
	visit_compound_stmt(*node.body);
	sym_tracker.pop_scope();
}

// Statements

void analyser::visit_decl_stmt(decl_stmt& node)
{
	return visit_decl(*node.inner_decl);
}

void analyser::visit_assign_stmt(assign_stmt& node)
{
	//! Point of interest
	visit_expr(*node.target);
	if (node.target->cat != exprcat::lval)
	{
		diagnostic(!soft_type_checks ? diagnostic_type::location_err : diagnostic_type::location_warning)
			.at(node.loc)
			.reason("Assigning to a non-object value")
			.report(diagnostics);
	}
	// I'll make it better, I swear
	else if (!node.target->type.exttp.empty()
		and node.target->type.exttp.back() == static_cast<std::byte>(dtypemod::_const))
	{
		diagnostic(!soft_type_checks ? diagnostic_type::location_err : diagnostic_type::location_warning)
			.at(node.loc)
			.reason("Assigning to a constant value")
			.report(diagnostics);
	}
	else
	{
		// TODO: Convert from expression type to variable type
	}

	visit_expr(*node.value);
}

void analyser::visit_compound_stmt(compound_stmt& node)
{
	auto* sym = sym_tracker.decl_sym();
	sym->is_global = false;
	sym->has_storage = false;
	sym_tracker.push_scope(sym);
	for (auto& s : node.body)
	{
		visit_stmt(*s);
	}
	sym_tracker.pop_scope();
}

void analyser::visit_ret_stmt(ret_stmt& node)
{
	visit_expr(*node.val);
	// TODO: Convert from expression type to return type
}

void analyser::visit_conditional_stmt(conditional_stmt& node)
{
	visit_expr(*node.cond);
	visit_stmt(*node.true_branch);
	if (node.false_branch)
		visit_stmt(*node.false_branch);
}

void analyser::visit_iteration_stmt(iteration_stmt& node)
{
	visit_expr(*node.cond);
	visit_stmt(*node.loop_body);
}

void analyser::visit_expr_stmt(expr_stmt& node)
{
	return visit_expr(*node.node);
}

void analyser::visit_null_stmt(stmt& node)
{}
