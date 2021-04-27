#pragma once
#include "tracker.hpp"
#include "../ast/ast.hpp"
#include "../shared/helper.hpp"
#include <string>
#include <vector>

class codegen
{
    public:
    void gen();
     
    void set_tree(ast_root*, std::string);
    void set_tracker(tracker*);
    
    // The name is pretty bad ngl
    std::string get_result();

    bool errored = false;
    std::vector<diagnostic> diagnostics;
    private:
    std::string emit_ident(identifier&);
    std::string emit_datatype(dtype&);
    std::string emit_literal(std::shared_ptr<literal_node>);

    std::string emit_args(arguments&);
    std::string emit_fcall(func_call_stmt&);

    std::string emit_subexpr(subexpr&);
    std::string emit_operand(operand&);
    std::string emit_expr(expr&);

    std::string emit_decl(decl_stmt&);
    std::string emit_def(def_stmt&);
    std::string emit_decldef(decldef_stmt&);

    std::string emit_ret(ret_stmt&);

    std::string emit_stmt(std::shared_ptr<stmt>);
    std::string emit_compound(compound_stmt&);

    std::string emit_entry(entry_stmt&);
    void gen_toplevel();

    ast_root* m_tree;
    tracker* m_tracker;
    std::string result;
    std::string filename;
};
