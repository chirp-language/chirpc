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
		case expr_kind::alloca:
			return visit_alloca_expr(static_cast<alloca_expr&>(node));
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

	if (node.left->has_error() or node.right->has_error())
	{
		node.cat = exprcat::error;
		return;
	}

	node.left = convert_to_rvalue(std::move(node.left));
	node.right = convert_to_rvalue(std::move(node.right));
	if (node.left->cat == exprcat::error or node.right->cat == exprcat::error)
	{
		node.cat = exprcat::error;
		return;
	}

	if (node.op >= tkn_type::cmp_S and node.op <= tkn_type::cmp_E)
	{
		if (node.left->type != node.right->type)
		{
			auto& t_lhs = node.left->type;
			auto& t_rhs = node.right->type;
			dtypeclass c_lhs = t_lhs.to_class(), c_rhs = t_rhs.to_class();
			if (c_lhs != c_rhs)
				goto _bad_types;
			else if (c_lhs == dtypeclass::_int)
			{
				basic_type common_type(dtypename::_int);
				if (t_lhs.basetp == dtypename::_long or t_rhs.basetp == dtypename::_long)
					common_type.basetp = dtypename::_long;
				if (t_lhs.has_modifier_front(dtypemod::_unsigned) or t_rhs.has_modifier_front(dtypemod::_unsigned))
					common_type.exttp.push_back(dtypemod::_unsigned);
				else if (t_lhs.has_modifier_front(dtypemod::_signed) or t_rhs.has_modifier_front(dtypemod::_signed))
					common_type.exttp.push_back(dtypemod::_signed);
				node.left = perform_implicit_conversions(
					std::move(node.left), common_type, exprcat::rval);
				node.right = perform_implicit_conversions(
					std::move(node.right), common_type, exprcat::rval);
				if (node.left->has_error() or node.right->has_error())
				{
					node.cat = exprcat::error;
					return;
				}
			}
			else if (c_lhs == dtypeclass::_float)
			{
				if (t_lhs.basetp != dtypename::_double and t_rhs.basetp != dtypename::_double)
					chirp_unreachable("Something's wrong, types don't match and neither value is a double");
				basic_type common_type(dtypename::_double);
				node.left = perform_implicit_conversions(
					std::move(node.left), common_type, exprcat::rval);
				node.right = perform_implicit_conversions(
					std::move(node.right), common_type, exprcat::rval);
				if (node.left->has_error() or node.right->has_error())
				{
					node.cat = exprcat::error;
					return;
				}
			}
			else if (c_lhs == dtypeclass::_ptr)
			{
				// For now, let them be
				diagnostic(diagnostic_type::location_warning)
					.at(node.loc)
					.reason("Pointer operands have different types")
					.report(diagnostics);
			}
		}
		node.cat = exprcat::rval;
		node.type.basetp = dtypename::_bool;
	}
	else
	{
		// TODO: Support pointer arithmetic
		if (node.left->type != node.right->type)
		{
			auto& t_lhs = node.left->type;
			auto& t_rhs = node.right->type;
			dtypeclass c_lhs = t_lhs.to_class(), c_rhs = t_rhs.to_class();
			// Promote to int
			if (c_lhs == dtypeclass::_bool)
				c_lhs = dtypeclass::_int;
			if (c_rhs == dtypeclass::_bool)
				c_rhs = dtypeclass::_int;
			if (c_lhs != c_rhs)
				goto _bad_types;
			else if (c_lhs == dtypeclass::_int)
			{
				basic_type common_type(dtypename::_int);
				if (t_lhs.basetp == dtypename::_long or t_rhs.basetp == dtypename::_long)
					common_type.basetp = dtypename::_long;
				if (t_lhs.has_modifier_front(dtypemod::_unsigned) or t_rhs.has_modifier_front(dtypemod::_unsigned))
					common_type.exttp.push_back(dtypemod::_unsigned);
				else if (t_lhs.has_modifier_front(dtypemod::_signed) or t_rhs.has_modifier_front(dtypemod::_signed))
					common_type.exttp.push_back(dtypemod::_signed);
				node.left = perform_implicit_conversions(
					std::move(node.left), common_type, exprcat::rval);
				node.right = perform_implicit_conversions(
					std::move(node.right), common_type, exprcat::rval);
				if (node.left->has_error() or node.right->has_error())
				{
					node.cat = exprcat::error;
					return;
				}
			}
			else if (c_lhs == dtypeclass::_float)
			{
				if (t_lhs.basetp != dtypename::_double and t_rhs.basetp != dtypename::_double)
					chirp_unreachable("Something's wrong, types don't match and neither value is a double");
				basic_type common_type(dtypename::_double);
				node.left = perform_implicit_conversions(
					std::move(node.left), common_type, exprcat::rval);
				node.right = perform_implicit_conversions(
					std::move(node.right), common_type, exprcat::rval);
				if (node.left->has_error() or node.right->has_error())
				{
					node.cat = exprcat::error;
					return;
				}
			}
			else if (c_lhs == dtypeclass::_ptr)
			{
				diagnostic(diagnostic_type::location_err)
					.at(node.loc)
					.reason("Arithmetic is not supported on pointers")
					.report(diagnostics);
				node.cat = exprcat::error;
				return;
			}
		}
		node.cat = exprcat::rval;
		node.type = node.left->type;
	}
	return;

	_bad_types:
	node.cat = exprcat::error;
	diagnostic(!soft_type_checks ? diagnostic_type::location_err : diagnostic_type::location_warning)
		.at(node.op_loc)
		.reason("Operand types don't match")
		.report(diagnostics);
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
		{
			auto& operand = *node.operand;
			node.cat = exprcat::rval;
			// Do promotion
			auto promoted_expr = promote_value(std::move(node.operand));
			if (promoted_expr->has_error())
				node.cat = exprcat::error;
			node.type = promoted_expr->type;
			node.operand = std::move(promoted_expr);
			break;
		}
		case tkn_type::minus_op:
		{
			auto& operand = *node.operand;
			node.cat = exprcat::rval;
			// Check that the type is an integer or a float, do promotion
			auto promoted_expr = promote_value(std::move(node.operand));
			if (promoted_expr->has_error())
				node.cat = exprcat::error;
			else if (promoted_expr->type.to_class() != dtypeclass::_int and
				promoted_expr->type.to_class() != dtypeclass::_float)
			{
				diagnostic(diagnostic_type::location_err)
					.at(node.loc)
					.reason("Cannot negate a non-number")
					.report(this->diagnostics);
				node.cat = exprcat::error;
				break;
			}
			node.type = promoted_expr->type;
			node.operand = std::move(promoted_expr);
			break;
		}
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
			node.type.exttp.push_back(dtypemod::_ptr);
			break;
		case tkn_type::deref_op:
			if (node.operand->type.exttp.empty() or node.operand->type.exttp.back() != dtypemod::_ptr)
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
		default:
			chirp_unreachable("visit_unop");
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

	node.type = node.callee->type;
	if (!node.type.exttp.empty() and node.type.exttp.back() == dtypemod::_func)
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

	visit_arguments(node.args);
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
				node.type.exttp.push_back(dtypemod::_func);
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
	node.type.exttp.push_back(dtypemod::_const);
	node.type.exttp.push_back(dtypemod::_ptr);
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

void analyser::visit_alloca_expr(alloca_expr& node)
{
	basic_type size_type(dtypename::_long);
	size_type.exttp.push_back(dtypemod::_unsigned);
	auto promoted_size = perform_implicit_conversions(
		std::move(node.size), size_type, exprcat::rval);
	if (promoted_size->cat == exprcat::error)
	{
		node.cat = exprcat::error;
	}
	else
	{
		node.cat = exprcat::rval;
		node.type.basetp = dtypename::_none;
		node.type.exttp.push_back(dtypemod::_ptr);
	}
	node.size = std::move(promoted_size);
	node.type = node.alloc_type;
	node.type.exttp.push_back(dtypemod::_ptr);
}

exprh analyser::convert_to_rvalue(exprh source)
{
	basic_type type = source->type;
	if (type.has_modifier_back(dtypemod::_const))
		type.exttp.pop_back();
	return perform_implicit_conversions(
		std::move(source), type, exprcat::rval);
}

exprh analyser::promote_value(exprh source)
{
	source = convert_to_rvalue(std::move(source));
	if (source->has_error())
		return source;

	basic_type new_type = source->type;
	if (new_type.to_class() == dtypeclass::_int)
	{
		if (new_type.basetp != dtypename::_long)
			new_type.basetp = dtypename::_int;
	}
	else if (new_type.to_class() == dtypeclass::_float)
	{
		// Nothing to be done
	}

	return perform_implicit_conversions(
		std::move(source), new_type, exprcat::rval);
}

// This function tries to perform implicit conversions to convert an expression to the desired type
// On failure to do so, returns null
exprh analyser::perform_implicit_conversions(exprh source, basic_type const& target_type, exprcat target_cat)
{
	dtypeclass cl_src = source->type.to_class(), cl_dst = target_type.to_class();
	if (cl_src == cl_dst)
	{
		// Check if types are equal
		if (target_type != source->type)
		{
			// First, remove const if necessary...
			if (source->type.has_modifier_back(dtypemod::_const))
			{
				auto const_cast_ = new_node<cast_expr>(cast_kind::_const);
				const_cast_->loc = source->loc;
				const_cast_->type.basetp = source->type.basetp;
				for (auto mod : source->type.exttp)
					if (mod != dtypemod::_const)
						const_cast_->type.exttp.push_back(mod);
				const_cast_->cat = exprcat::rval;
				const_cast_->operand = std::move(source);
				source = std::move(const_cast_);
			}

			// Integral conversions
			// Convert between different operand sizes
			// dtypeclass none shouldn't be passed in the first place
			if (cl_dst == dtypeclass::_int)
			{
				// For now, just convert directly to the target type, then match signedness

				if (source->type.basetp != target_type.basetp)
				{
					auto grade_cast = new_node<cast_expr>(cast_kind::_grade);
					grade_cast->loc = source->loc;
					grade_cast->type.basetp = target_type.basetp;
					for (auto mod : source->type.exttp)
						grade_cast->type.exttp.push_back(mod);
					grade_cast->cat = exprcat::rval;
					grade_cast->operand = std::move(source);
					source = std::move(grade_cast);
				}

				if (target_type.has_modifier_back(dtypemod::_signed) and !source->type.has_modifier_back(dtypemod::_signed))
				{
					auto sign_cast = new_node<cast_expr>(cast_kind::_sign);
					sign_cast->loc = source->loc;
					sign_cast->type.basetp = target_type.basetp;
					sign_cast->type.exttp.push_back(dtypemod::_signed);
					sign_cast->cat = exprcat::rval;
					sign_cast->operand = std::move(source);
					source = std::move(sign_cast);
				}
				else if (target_type.has_modifier_back(dtypemod::_unsigned) and !source->type.has_modifier_back(dtypemod::_unsigned))
				{
					auto sign_cast = new_node<cast_expr>(cast_kind::_sign);
					sign_cast->loc = source->loc;
					sign_cast->type.basetp = target_type.basetp;
					sign_cast->type.exttp.push_back(dtypemod::_unsigned);
					sign_cast->cat = exprcat::rval;
					sign_cast->operand = std::move(source);
					source = std::move(sign_cast);
				}

				// We're done
			}
			else if (cl_dst == dtypeclass::_float)
			{
				auto grade_cast = new_node<cast_expr>(cast_kind::_grade);
				grade_cast->loc = source->loc;
				grade_cast->type.basetp = target_type.basetp;
				// No modifiers to add
				grade_cast->cat = exprcat::rval;
				grade_cast->operand = std::move(source);
				source = std::move(grade_cast);
			}
			else if (cl_dst == dtypeclass::_bool)
			{
				// Shouldn't happen!
				chirp_unreachable("Conversion from bool to bool not needed");
			}
			else if (cl_dst == dtypeclass::_ptr)
			{
				// TODO
				diagnostic(!soft_type_checks ? diagnostic_type::location_err : diagnostic_type::location_warning)
					.at(source->loc)
					.reason("Cannot convert between pointer types (yet)")
					.report(this->diagnostics);
				goto _gen_error;
			}
			else if (cl_dst == dtypeclass::_func)
			{
				diagnostic(diagnostic_type::location_err)
					.at(source->loc)
					.reason("Cannot convert function types")
					.report(this->diagnostics);
				goto _gen_error;
			}

			// ...and add it back if required
			if (target_type.has_modifier_back(dtypemod::_const))
			{
				auto const_cast_ = new_node<cast_expr>(cast_kind::_const);
				const_cast_->loc = source->loc;
				const_cast_->type.basetp = target_type.basetp;
				for (auto mod : source->type.exttp)
					const_cast_->type.exttp.push_back(mod);
				const_cast_->type.exttp.push_back(dtypemod::_const);
				const_cast_->cat = exprcat::rval;
				const_cast_->operand = std::move(source);
				source = std::move(const_cast_);
			}
		}
		// No more conversions needed
	}
	// int <-> float conversions
	else if (cl_dst == dtypeclass::_float and cl_src == dtypeclass::_int
		or cl_dst == dtypeclass::_int and cl_src == dtypeclass::_float)
	{
		{
			// First, convert to a signed value (if int) and remove const
			basic_type clean_type = source->type;
			if (clean_type.has_modifier_back(dtypemod::_const))
				clean_type.exttp.pop_back();
			if (cl_src == dtypeclass::_int)
			{
				if (clean_type.has_modifier_back(dtypemod::_unsigned))
					clean_type.exttp.pop_back();
				if (!clean_type.has_modifier_back(dtypemod::_signed))
					clean_type.exttp.push_back(dtypemod::_signed);
			}
			auto clean_val = perform_implicit_conversions(
				std::move(source), clean_type, exprcat::rval);
			if (clean_val->cat == exprcat::error)
				return clean_val;
			source = std::move(clean_val);
		}
		// Next, perform the float conversion
		{
			auto float_cast = new_node<cast_expr>(cast_kind::_float);
			float_cast->loc = source->loc;
			float_cast->type = target_type;
			if (target_type.has_modifier_back(dtypemod::_const))
				float_cast->type.exttp.pop_back();
			if (float_cast->type.has_modifier_back(dtypemod::_signed) or
					float_cast->type.has_modifier_back(dtypemod::_unsigned))
				float_cast->type.exttp.pop_back();
			float_cast->cat = exprcat::rval;
			float_cast->operand = std::move(source);
			source = std::move(float_cast);
		}
		// Now, convert to the target type
		return perform_implicit_conversions(
			std::move(source), target_type, target_cat);
	}
	// boolean conversions
	else if (cl_dst == dtypeclass::_bool)
	{
		location_range src_loc = source->loc;
		switch (cl_src)
		{
			case dtypeclass::_int:
			case dtypeclass::_float:
			{
				// TODO: Add float literals
				exprh arg;
				{
					auto lit = new_node<integral_literal>();
					lit->value = 0;
					lit->cat = exprcat::rval;
					lit->type.basetp = dtypename::_int;
					lit->type.exttp.push_back(dtypemod::_signed);
					arg = std::move(lit);
				}

				if (cl_src == dtypeclass::_float)
					// Assume this doesn't fail
					arg = perform_implicit_conversions(
						std::move(arg), basic_type(dtypename::_float), exprcat::rval);

				auto comp = new_node<binop>(tkn_type::noteq_op, std::move(source), std::move(arg));
				comp->cat = exprcat::rval;
				comp->type.basetp = dtypename::_bool;
				source = std::move(comp);
				break;
			}
			case dtypeclass::_ptr:
			{
				auto arg = new_node<nullptr_literal>();
				arg->cat = exprcat::rval;
				arg->type = source->type;
				auto comp = new_node<binop>(tkn_type::noteq_op, std::move(source), std::move(arg));
				comp->cat = exprcat::rval;
				comp->type.basetp = dtypename::_bool;
				source = std::move(comp);
				break;
			}
			case dtypeclass::_bool:
			case dtypeclass::_none:
			case dtypeclass::_func:
				goto _no_conv;
		}
		// Construct cast node
		auto cast_node = new_node<cast_expr>(cast_kind::_bool);
		cast_node->loc = src_loc;
		cast_node->type = source->type;
		cast_node->cat = source->cat;
		cast_node->operand = std::move(source);
		source = std::move(cast_node);
	}
	else
	{
		_no_conv:
		diagnostic(!soft_type_checks ? diagnostic_type::location_err : diagnostic_type::location_warning)
			.at(source->loc)
			.reason("Cannot convert expression to expected type")
			.report(this->diagnostics);
		goto _gen_error;
	}

	// Perform value category conversions
	if (target_cat == exprcat::lval and source->cat == exprcat::rval)
	{
		diagnostic(diagnostic_type::location_err)
			.at(source->loc)
			.reason("Cannot convert an rvalue to lvalue")
			.report(this->diagnostics);

		// Provide a placeholder expression for debugging purposes
		_gen_error:
		auto err_expr = new_node<cast_expr>(cast_kind::_invalid);
		err_expr->loc = source->loc;
		err_expr->type = target_type;
		err_expr->cat = exprcat::error;
		err_expr->operand = std::move(source);
		return err_expr;
	}
	else if (target_cat == exprcat::rval and source->cat == exprcat::lval)
	{
		if (cl_src == dtypeclass::_func)
		{
			diagnostic(diagnostic_type::location_err)
				.at(source->loc)
				.reason("Cannot convert a function lvalue to rvalue")
				.report(this->diagnostics);
			goto _gen_error;
		}
		auto cat_cast = new_node<cast_expr>(cast_kind::_cat);
		cat_cast->loc = source->loc;
		cat_cast->type = target_type;
		cat_cast->cat = exprcat::rval;
		cat_cast->operand = std::move(source);
		return cat_cast;
	}
	return source;
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
		for (auto d : node.type.exttp)
		{
			if (d == dtypemod::_ptr)
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
	if (node.init)
	{
		visit_expr(*node.init);
		// Convert from initializer type (if any) to variable type
		if (node.init->cat != exprcat::error)
			node.init = perform_implicit_conversions(
				std::move(node.init), node.type, exprcat::rval);
	}
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
	visit_expr(*node.value);
	if (node.target->has_error())
		return;
	if (node.target->cat != exprcat::lval)
	{
		diagnostic(!soft_type_checks ? diagnostic_type::location_err : diagnostic_type::location_warning)
			.at(node.loc)
			.reason("Assigning to a non-object value")
			.report(diagnostics);
	}
	// I'll make it better, I swear
	else if (!node.target->type.exttp.empty()
		and node.target->type.exttp.back() == dtypemod::_const)
	{
		diagnostic(!soft_type_checks ? diagnostic_type::location_err : diagnostic_type::location_warning)
			.at(node.loc)
			.reason("Assigning to a constant value")
			.report(diagnostics);
	}
	else if (node.value->cat != exprcat::error)
	{
		// Convert from expression type to variable type
		node.value = perform_implicit_conversions(
			std::move(node.value), node.target->type, exprcat::rval);
	}
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
	if (node.cond->cat != exprcat::error)
	{
		node.cond = perform_implicit_conversions(
			std::move(node.cond), basic_type(dtypename::_bool), exprcat::rval);
	}
	visit_stmt(*node.true_branch);
	if (node.false_branch)
		visit_stmt(*node.false_branch);
}

void analyser::visit_iteration_stmt(iteration_stmt& node)
{
	visit_expr(*node.cond);
	if (node.cond->cat != exprcat::error)
	{
		node.cond = perform_implicit_conversions(
			std::move(node.cond), basic_type(dtypename::_bool), exprcat::rval);
	}
	visit_stmt(*node.loop_body);
}

void analyser::visit_expr_stmt(expr_stmt& node)
{
	return visit_expr(*node.node);
}

void analyser::visit_null_stmt(stmt& node)
{}
