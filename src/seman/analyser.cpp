#include "analyser.hpp"

// Dispatch functions

void analyser::analyse()
{
	for (auto& d : root.imports)
		visit_import_decl(*d);
	for (auto& d : root.externs)
		visit_extern_decl(*d);
	for (auto& d : root.fdecls)
		visit_func_decl(*d);
	for (auto& d : root.fdefs)
		visit_func_def(*d);
	if (root.entry)
		visit_entry_decl(*root.entry);
}

void analyser::visit_expr(expr& node)
{
	switch (node.kind)
	{
		case optype::lit:
			if (static_cast<literal_node&>(node).ltype == littype::txt)
				return visit_txt_literal(static_cast<txt_literal&>(node));
			return visit_num_literal(static_cast<num_literal&>(node));
		case optype::ident:
			return visit_id_ref_expr(static_cast<id_ref_expr&>(node));
		case optype::call:
			return visit_func_call(static_cast<func_call&>(node));
		case optype::op:
			return visit_binop(static_cast<binop&>(node));
		case optype::invalid:
		default:
			return;
	}
}

void analyser::visit_decl(decl& node)
{
	switch (node.type)
	{
		case decl_type::var:
			return visit_var_decl(static_cast<var_decl&>(node));
		case decl_type::entry:
			return visit_entry_decl(static_cast<entry_decl&>(node));
		case decl_type::import:
			return visit_import_decl(static_cast<import_decl&>(node));
		case decl_type::fdecl:
			return visit_func_decl(static_cast<func_decl&>(node));
		case decl_type::fdef:
			return visit_func_def(static_cast<func_def&>(node));
		case decl_type::external:
			return visit_extern_decl(static_cast<extern_decl&>(node));
		default:
			return;
	}
}

void analyser::visit_stmt(stmt& node)
{
	switch (node.type)
	{
		case stmt_type::decl:
			return visit_decl_stmt(static_cast<decl_stmt&>(node));
		case stmt_type::assign:
			return visit_assign_stmt(static_cast<assign_stmt&>(node));
		case stmt_type::compound:
			return visit_compound_stmt(static_cast<compound_stmt&>(node));
		case stmt_type::ret:
			return visit_ret_stmt(static_cast<ret_stmt&>(node));
		case stmt_type::conditional:
			return visit_conditional_stmt(static_cast<conditional_stmt&>(node));
		case stmt_type::iteration:
			return visit_iteration_stmt(static_cast<iteration_stmt&>(node));
		case stmt_type::expr:
			return visit_expr_stmt(static_cast<expr_stmt&>(node));
		case stmt_type::null:
			return visit_null_stmt(node);
		default:
			return;
	}
}

// Expressions

void analyser::visit_binop(binop& node)
{
	visit_expr(*node.left);
	visit_expr(*node.right);
	// TODO: Handle operation
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
		node.type.cattp = opcat::rval;
	}
	else
	{
		node.type.cattp = opcat::error;
	}
}

void analyser::visit_id_ref_expr(id_ref_expr& node)
{
	//! Point of interest
	if (auto sym = sym_tracker.lookup_sym(&node.ident))
	{
		node.target = sym;
		if (sym->type == decl_type::var)
		{
			node.type = static_cast<var_decl const&>(*sym).var_type;
			node.type.cattp = opcat::lval;
		}
		else if (sym->type == decl_type::fdecl or sym->type == decl_type::fdef)
		{
			node.type = static_cast<func_decl const&>(*sym).data_type;
			node.type.cattp = opcat::rval;
			node.type.exttp.push_back(static_cast<std::byte>(dtypemod::_func));
		}
		else
		{
			node.type.cattp = opcat::error;
			diagnostic d;
			d.type = diagnostic_type::location_err;
			d.l = node.loc;
			d.msg = "Unknown declaration type";
			diagnostics.show(d);
		}
	}
	else
	{
		node.type.cattp = opcat::error;
		if (!ignore_unresolved_refs)
		{
			diagnostic d;
			d.type = diagnostic_type::location_warning;
			d.l = node.loc;
			d.msg = "Referenced an undefined variable or a global function/variable (TODO)";
			diagnostics.show(d);
		}
	}
}

void analyser::visit_txt_literal(txt_literal& node)
{
	node.type.cattp = opcat::rval;
	node.type.basetp = dtypename::_char;
	node.type.exttp.push_back(static_cast<std::byte>(dtypemod::_const));
	node.type.exttp.push_back(static_cast<std::byte>(dtypemod::_ptr));
}

void analyser::visit_num_literal(num_literal& node)
{
	node.type.cattp = opcat::rval;
	node.type.basetp = dtypename::_int;
}

// Declarations

void analyser::visit_var_decl(var_decl& node)
{
	//! Point of interest
	if (!sym_tracker.bind_sym(&node.ident, &node))
	{
		diagnostic e;
		e.l = node.loc;
		e.msg = "A variable with the same name already exists";
		e.type = diagnostic_type::location_err;
		this->diagnostics.show(e);
		return;
	}

	// Check for invalid type
	if (node.var_type.basetp == dtypename::_none)
	{
		// Checks if it's a pointer.
		bool is_ptr = false;
		for (std::byte d : node.var_type.exttp)
		{
			if (static_cast<dtypemod>(d) == dtypemod::_ptr)
			{
				is_ptr = true;
				break;
			}
		}

		if (!is_ptr)
		{
			diagnostic e;
			e.msg = "Variable cannot be of type `none`, unless it's a pointer";
			e.type = diagnostic_type::location_err;
			e.l = node.loc;
			this->diagnostics.show(e);
		}
	}
	// TODO: Convert from initializer type (if any) to variable type
	if (node.init)
		visit_expr(*node.init);
}

void analyser::visit_entry_decl(entry_decl& node)
{
	visit_compound_stmt(static_cast<compound_stmt&>(*node.code));
}

void analyser::visit_import_decl(import_decl& node)
{
	// TODO: Bind name
}

void analyser::visit_extern_decl(extern_decl& node)
{
	// TODO: Bind name
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

	if (!sym_tracker.bind_sym(&node.ident, &node))
	{
		// TODO: Check if declarations match
		diagnostic d;
		d.type = diagnostic_type::location_err;
		d.l = node.loc;
		d.msg = "A symbol with that name already exists";
		diagnostics.show(d);
	}

	sym_tracker.push_scope();
	visit_parameters(node.params);
	sym_tracker.pop_scope();
}

void analyser::visit_func_def(func_def& node)
{
	//! Point of interest

	if (auto sym = sym_tracker.find_sym_cur(&node.ident))
	{
		// TODO: Check if declarations match
		if (sym->target->type == decl_type::fdecl)
		{
			// Rebind the symbol to point to definition
			sym->target = &node;
		}
		else
		{
			diagnostic d;
			d.type = diagnostic_type::location_err;
			d.l = node.loc;
			d.msg = "A symbol with that name already exists";
			diagnostics.show(d);
		}
	}
	else
	{
		sym_tracker.push_sym_unsafe(&node.ident, &node);
	}

	sym_tracker.push_scope();
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
	if (auto var = sym_tracker.lookup_sym(&node.ident))
	{
		if (var->type == decl_type::var)
		{
			node.target = static_cast<var_decl const*>(var);
			// TODO: Convert from expression type to variable type
		}
		else
		{
			diagnostic d;
			d.type = diagnostic_type::location_err;
			d.l = node.loc;
			d.msg = "Assigning to a non-variable";
			diagnostics.show(d);
		}
	}
	else
	{
		diagnostic d;
		d.type = diagnostic_type::location_err;
		d.l = node.loc;
		d.msg = "Assigning to an undefined variable";
		diagnostics.show(d);
	}

	visit_expr(*node.value);
}

void analyser::visit_compound_stmt(compound_stmt& node)
{
	sym_tracker.push_scope();
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
