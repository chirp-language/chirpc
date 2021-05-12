#pragma once
#include "tracker.hpp"
#include "../ast/ast.hpp"
#include "../shared/diagnostic.hpp"
#include <string>
#include <vector>

class codegen
{
    public:
    void gen();

    void set_tree(ast_root*, std::string);
    void set_tracker(tracker*);
    
    // The name is pretty bad ngl
    std::string&& get_result();

    bool errored = false;
    std::vector<diagnostic> diagnostics;
    private:

    std::string emit_datatype(exprtype const&);
    std::string emit_expr(expr const&);
    std::string emit_binop(binop const&);
    std::string emit_arguments(arguments const&);
    std::string emit_func_call(func_call const&);
    std::string emit_identifier(identifier const&);
    std::string emit_literal(literal_node const&);

    std::string emit_decl(decl const&);
    std::string emit_var_decl(var_decl const&);
    std::string emit_entry_decl(entry_decl const&);

    std::string emit_stmt(stmt const&);
    std::string emit_assign_stmt(assign_stmt const&);
    std::string emit_compound_stmt(compound_stmt const&);
    std::string emit_ret_stmt(ret_stmt const&);

    void gen_toplevel();

    ast_root* m_tree;
    tracker* m_tracker;
    std::string result;
    std::string filename;
};
