// This whole file is dedicated for parsing expressions
// DeKrain's Improved Expression Parsing "Algorithm"® a.k.a. Operator-precedence parser
#include "parser.hpp"

bool parser::is_binop()
{
    return (
        is_identifier()            ||
        probe(tkn_type::literal)   ||
        probe_range(tkn_type::binop_S,
            tkn_type::binop_E)     ||
        probe(tkn_type::lparen)    ||
        probe(tkn_type::rparen));
}

enum class precedence_class {
    group,
    fcall,
    ref,
    prod,
    sum,
    as,
    cmp,
    comma,
};

static int get_operator_precedence(tkn_type op)
{
    // TODO: Use map
    switch (op) {
        case tkn_type::lparen:
            return static_cast<int>(precedence_class::fcall);
        case tkn_type::ref_op:
        case tkn_type::deref_op:
            return static_cast<int>(precedence_class::ref);
        case tkn_type::as_op:
            return static_cast<int>(precedence_class::as);
        case tkn_type::star_op:
        case tkn_type::slash_op:
        case tkn_type::perc_op:
            return static_cast<int>(precedence_class::prod);
        case tkn_type::plus_op:
        case tkn_type::minus_op:
            return static_cast<int>(precedence_class::sum);
        case tkn_type::lt_op:
        case tkn_type::gt_op:
        case tkn_type::lteq_op:
        case tkn_type::gteq_op:
        case tkn_type::eqeq_op:
        case tkn_type::noteq_op:
            return static_cast<int>(precedence_class::cmp);
        default:
            return -1u >> 1;
    }
}

static bool is_operator(tkn_type t)
{
    return (
        t >= tkn_type::binop_S and t <= tkn_type::binop_E or
        t == tkn_type::as_op or t == tkn_type::ref_op or
        t == tkn_type::lparen // func_call
    );
}

exprh parser::get_subexpr_op(exprh lhs, int max_prec)
{
    tkn_type optype;

    while (is_operator(optype = peek().type))
    {
        auto lop = loc_peek();
        skip();
        int pr = get_operator_precedence(optype);
        if (pr > max_prec)
            return lhs;
        if (optype == tkn_type::lparen)
        {
            // Parse arguments and combine
            lhs = get_fcall(std::move(lhs));
            continue;
        }
        if (optype == tkn_type::as_op)
        {
            // Cast to new type
            bool has_paren = match(tkn_type::lparen);
            basic_type newtp = get_datatype();
            if (has_paren)
                expect(tkn_type::rparen);
            auto ecast = new_node<cast_expr>();
            ecast->operand = std::move(lhs);
            ecast->type = std::move(newtp);
            ecast->cat = exprcat::rval;
            lhs = std::move(ecast);
            continue;
        }
        exprh rhs = get_primary_expr();
        tkn_type tmpop;
        while (is_operator(tmpop = peek().type) and get_operator_precedence(tmpop) < pr)
        {
            rhs = get_subexpr_op(std::move(rhs), max_prec - 1);
        }
        auto lbeg = lhs->loc.begin;
        auto lend = rhs->loc.end;
        lhs = new_node<binop>(optype, std::move(lhs), std::move(rhs));
        lhs->loc = location_range(lbeg, lend);
        static_cast<binop&>(*lhs).op_loc = lop;
    }
    return lhs;
}

exprh parser::get_primary_expr()
{
    using namespace std::string_literals;

    if (is_identifier())
    {
        return id_ref_expr::from(get_identifier());
    }
    else if (probe(tkn_type::literal))
    {
        return get_literal();
    }
    else if (probe(tkn_type::kw_true) or probe(tkn_type::kw_false))
    {
        return new_node<num_literal>(get_bool_lit());
    }
    else if (match(tkn_type::kw_null))
    {
        return new_node<num_literal>(get_null_ptr_lit());
    }
    else if (match(tkn_type::lparen))
    {
        // Parenthesis are special, as they aren't considered as operations, but as sub_expressions
        // Because of this, the location of the parenthesis is not stored in the tree
        exprh result = get_expr(true);
        expect(tkn_type::rparen);
        return result;
    }
            this->ok = false;
            diagnostic(diagnostic_type::location_err)
                .at(loc_peek())
                .reason("Expected expression")
                .report(this->diagnostics);
            return nullptr;
    }
}

exprh parser::get_expr(bool comma_allowed)
{
    if (exprh lhs = get_primary_expr())
        return get_subexpr_op(std::move(lhs), comma_allowed ? static_cast<int>(precedence_class::comma) : static_cast<int>(precedence_class::comma) - 1);
    return nullptr;
}
