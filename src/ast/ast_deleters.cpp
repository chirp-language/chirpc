/// \file Deleters for AST nodes

#include "ast.hpp"

void expr_node_deleter::operator()(expr* node) const
{
	switch (node->kind)
	{
		case expr_kind::binop:
			return delete static_cast<binop*>(node);
		case expr_kind::unop:
			return delete static_cast<unop*>(node);
		case expr_kind::call:
			return delete static_cast<func_call*>(node);
		case expr_kind::ident:
			return delete static_cast<id_ref_expr*>(node);
		case expr_kind::strlit:
			return delete static_cast<string_literal*>(node);
		case expr_kind::intlit:
			return delete static_cast<integral_literal*>(node);
		case expr_kind::nulllit:
			return delete static_cast<nullptr_literal*>(node);
		case expr_kind::cast:
			return delete static_cast<cast_expr*>(node);
	}
}

void decl_node_deleter::operator()(decl* node) const
{
	switch (node->kind)
	{
		case decl_kind::root:
			return;
		case decl_kind::var:
			return delete static_cast<var_decl*>(node);
		case decl_kind::entry:
			return delete static_cast<entry_decl*>(node);
		case decl_kind::import:
			return delete static_cast<import_decl*>(node);
		case decl_kind::nspace:
			return delete static_cast<namespace_decl*>(node);
		case decl_kind::fdecl:
			return delete static_cast<func_decl*>(node);
		case decl_kind::fdef:
			return delete static_cast<func_def*>(node);
		case decl_kind::external:
			return delete static_cast<extern_decl*>(node);
	}
}

void stmt_node_deleter::operator()(stmt* node) const
{
	switch (node->kind)
	{
		case stmt_kind::decl:
			return delete static_cast<decl_stmt*>(node);
		case stmt_kind::assign:
			return delete static_cast<assign_stmt*>(node);
		case stmt_kind::compound:
			return delete static_cast<compound_stmt*>(node);
		case stmt_kind::ret:
			return delete static_cast<ret_stmt*>(node);
		case stmt_kind::conditional:
			return delete static_cast<conditional_stmt*>(node);
		case stmt_kind::iteration:
			return delete static_cast<iteration_stmt*>(node);
		case stmt_kind::expr:
			return delete static_cast<expr_stmt*>(node);
		case stmt_kind::null:
			return delete static_cast<null_stmt*>(node);
	}
}
