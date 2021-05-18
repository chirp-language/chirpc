/// \file Deleters for AST nodes

#include "ast.hpp"

void expr_node_deleter::operator()(expr* node) const
{
	switch (node->kind)
	{
		case optype::lit:
			if (static_cast<literal_node&>(*node).ltype == littype::txt)
				return delete static_cast<txt_literal*>(node);
			return delete static_cast<num_literal*>(node);
		case optype::ident:
			return delete static_cast<id_ref_expr*>(node);
		case optype::call:
			return delete static_cast<func_call*>(node);
		case optype::op:
			return delete static_cast<binop*>(node);
		case optype::invalid:
		default:
			return;
	}
}

void decl_node_deleter::operator()(decl* node) const
{
	switch (node->type)
	{
		case decl_type::var:
			return delete static_cast<var_decl*>(node);
		case decl_type::entry:
			return delete static_cast<entry_decl*>(node);
		case decl_type::import:
			return delete static_cast<import_decl*>(node);
		case decl_type::fdecl:
			return delete static_cast<func_decl*>(node);
		case decl_type::fdef:
			return delete static_cast<func_def*>(node);
		case decl_type::external:
			return delete static_cast<extern_decl*>(node);
		default:
			return;
	}
}

void stmt_node_deleter::operator()(stmt* node) const
{
	switch (node->type)
	{
		case stmt_type::decl:
			return delete static_cast<decl_stmt*>(node);
		case stmt_type::assign:
			return delete static_cast<assign_stmt*>(node);
		case stmt_type::compound:
			return delete static_cast<compound_stmt*>(node);
		case stmt_type::ret:
			return delete static_cast<ret_stmt*>(node);
		case stmt_type::conditional:
			return delete static_cast<conditional_stmt*>(node);
		case stmt_type::iteration:
			return delete static_cast<iteration_stmt*>(node);
		case stmt_type::expr:
			return delete static_cast<expr_stmt*>(node);
		case stmt_type::null:
			return delete static_cast<null_stmt*>(node);
		default:
			return;
	}
}
