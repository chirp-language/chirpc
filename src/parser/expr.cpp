// This whole file is dedicated for parsing expressions
// DeKrain's Improved Expression Parsing "Algorithm"® a.k.a. Operator-precedence parser
#include "parser.hpp"

bool parser::is_operand()
{
    return (
        is_identifier()            ||
        probe(tkn_type::literal)    ||
        probe(tkn_type::math_op)    ||
        probe(tkn_type::deref_op)   ||
        probe(tkn_type::ref_op)     ||
        probe(tkn_type::as_op)      ||
        probe(tkn_type::lparen)     ||
        probe(tkn_type::rparen));
}

enum class precedence_class {
    group,
    fcall,
    ref,
    prod,
    sum,
    comma,
};

static int get_operator_precedence(exprop op)
{
    // TODO: Use map
    switch (op) {
        case exprop::call:
            return static_cast<int>(precedence_class::fcall);
        case exprop::as:
        case exprop::deref:
        case exprop::ref:
            return static_cast<int>(precedence_class::ref);
        case static_cast<exprop>('*'):
        case static_cast<exprop>('/'):
            return static_cast<int>(precedence_class::prod);
        case static_cast<exprop>('+'):
        case static_cast<exprop>('-'):
            return static_cast<int>(precedence_class::sum);
    }
    return -1;
}

static exprop get_operator_type(token const& t)
{
    switch (t.type)
    {
        case tkn_type::math_op:
            return static_cast<exprop>(static_cast<unsigned char>(t.value.front()));
        case tkn_type::as_op:
            return exprop::as;
        case tkn_type::deref_op:
            return exprop::deref;
        case tkn_type::ref_op:
            return exprop::ref;
        case tkn_type::lparen:
            return exprop::call;
        case tkn_type::cmp_op:
            // TODO
        case tkn_type::assign_op:
            // TODO
        case tkn_type::comma:
            // TODO: Comma expression OR list element
        case tkn_type::period:
            // TODO: Member access ig
        default:
            return exprop::none;
    }
}

exprh parser::get_subexpr_op(exprh lhs, int max_prec)
{
    exprop optype;

    while ((optype = get_operator_type(peek())) != exprop::none)
    {
        auto lop = loc_peek();
        skip();
        int pr = get_operator_precedence(optype);
        if (pr > max_prec)
            return lhs;
        if (optype == exprop::call) {
            // Parse arguments and combine
            lhs = get_fcall(std::move(lhs));
            continue;
        }
        exprh rhs = get_primary_expr();
        exprop tmpop;
        while (tmpop = get_operator_type(peek()), tmpop != exprop::none and get_operator_precedence(tmpop) < pr) {
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
    else if (probe(tkn_type::lparen))
    {
        // Parenthesis are special, as they aren't considered as operations, but as sub_expressions
        // Because of this, the location of the parenthesis is not stored in the tree
        skip();
        exprh result = get_expr(true);
        expect(tkn_type::rparen);
        return result;
    }
    else
    {
        diagnostic e;
        e.l = loc_peek();
        e.msg = "Invalid operand";
        e.type = diagnostic_type::location_err;
        this->ok = false;
        this->diagnostics.show(e);
        return nullptr;
    }
}

exprh parser::get_expr(bool comma_allowed)
{
    if (exprh lhs = get_primary_expr())
        return get_subexpr_op(std::move(lhs), comma_allowed ? static_cast<int>(precedence_class::comma) : static_cast<int>(precedence_class::comma) - 1);
    return nullptr;
}
