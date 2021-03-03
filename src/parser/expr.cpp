// This whole file is dedicated for parsing expressions
// Bink's Sketchy Expression Parsing "Algorithm"®
// 1 - When parsing an expression, get the range of the expression,
// and turn all the components of the expressions into operands
// 2 - Go at the last operand of the expression, and go reverse, to find
// the operator with lowest precedence
// 3 - Split the range in two halfs, and do the same thing for these
// (also check if there's even an operator, otherwise just end there)
// This is probably already an existing algorithm, I just improvised something lol
#include "parser.hpp"

#include <iostream>

bool parser::is_operand(bool reset)
{
    bool result = false;
    int op = this->cursor;
    if(
        is_func_call(false)||
        is_identifier(false)||
        match(tkn_type::literal)||
        match(tkn_type::math_op)||
        match(tkn_type::deref_op)||
        match(tkn_type::ref_op)||
        match(tkn_type::as_op)||
        match(tkn_type::lparen)||
        match(tkn_type::rparen)
        )
    {
        result = true;
    }
    if(reset)
    {
        this->cursor = op;
    }
    return result;
}

subexpr parser::get_subexpr(std::vector<operand> range)
{
    subexpr node;

    // ()   0
    // as, deref, ref   1
    // * /   2
    // + -   3
    int op_index;
    int op_rank = -1; // bad name kinda
    char op_char;
    bool has_op = false;
    
    for(int i=range.size()-1;i>0;i--)
    {
        operand& operand = range.at(i);
        
        if(operand.type == optype::op)
        {
            has_op = true;
            exprop op = *static_cast<exprop*>(operand.node.get());
            std::cout<<op.type<<std::endl;
            if(op.type == '(' || op.type == ')'){
                if(op_rank < 0){
                    op_rank = 0;
                    op_char = op.type;
                    op_index = i;
                }
            }
            else if(op.type == 'a' || op.type == 'd' || op.type == 'r'){
                if(op_rank < 1){
                    op_rank = 1;
                    op_char = op.type;
                    op_index = i;
                }
            }
            else if(op.type == '*' || op.type == '/'){
                if(op_rank < 2){
                    op_rank = 2;
                    op_char = op.type;
                    op_index = i;
                }
            }
            else if(op.type == '+' || op.type == '-'){
                if(op_rank < 3){
                    op_rank = 3;
                    op_char = op.type;
                    op_index = i;
                }
            }
            else{
                this->ok = false;
            }
        }
    }

    if(!has_op)
    {
        helper e;
        e.msg = "(W.I.P) Expression with mutliple terms and no operator";
        e.type = helper_type::global_err;
        this->ok = false;
        this->helpers.push_back(e);
    }
    else
    {
        // Left Side
        std::cout<<"Left"<<std::endl;
        std::vector<operand> lrange;
        for(int i = 0; i < op_index; i++)
        {
            lrange.push_back(range.at(i));
            std::cout<<range.at(i).dump(0);
        }

        if(lrange.size() == 1)
        {
            node.left = lrange.at(0);
        }
        else
        {
            operand o;
            o.type = optype::subexpr;
            o.node = std::make_shared<subexpr>(get_subexpr(lrange));
            node.left = o;
        }

        // Right side
        std::cout<<"Right"<<std::endl;
        std::vector<operand> rrange;
        for(int i = op_index + 1; i < range.size(); i++)
        {
            rrange.push_back(range.at(i));
            std::cout<<range.at(i).dump(0);
        }

        if(rrange.size() == 1)
        {
            node.right = rrange.at(0);
        }
        else
        {
            operand o;
            o.type = optype::subexpr;
            o.node = std::make_shared<subexpr>(get_subexpr(rrange));
            node.right = o;
        }

        node.op = *static_cast<exprop*>(range.at(op_index).node.get());
    }

    return node;
}

operand parser::get_operand()
{
    operand result;
    if(is_func_call(true))
    {
        result.type = optype::call;
        result.node = std::make_shared<func_call_stmt>(get_fcall());
    }
    else if(is_identifier(true))
    {
        result.type = optype::ident;
        result.node = std::make_shared<identifier>(get_identifier());
    }
    else if(peek().type == tkn_type::literal)
    {
        result.type = optype::lit;
        result.node = get_literal();
    }
    else
    {
        std::string v = peek().value;
        result.type = optype::op;
        exprop op;
        if(v == "("){
            op.type = '(';
            expect(tkn_type::lparen);
        }
        else if(v == ")"){
            op.type = ')';
            expect(tkn_type::rparen);
        }
        else if(v == "*"){
            op.type = '*';
            expect(tkn_type::math_op);
        }
        else if(v == "/"){
            op.type = '/';
            expect(tkn_type::math_op);
        }
        else if(v == "+"){
            op.type = '+';
            expect(tkn_type::math_op);
        }
        else if(v == "-"){
            op.type = '-';
            expect(tkn_type::math_op);
        }
        else if(v == "deref"){
            expect(tkn_type::deref_op);
        }
        else if(v == "ref"){
            expect(tkn_type::ref_op);
        }
        else if(v == "as"){
            expect(tkn_type::as_op);
        }
        else{
            result.type = optype::invalid;
            helper e;
            e.l = peek().loc;
            e.msg = "Invalid operand";
            std::cout<<"Not an operand"<<std::endl;
            e.type = helper_type::location_err;
            this->ok = false;
            this->helpers.push_back(e);
            return result;
        }
        result.node = std::make_shared<exprop>(op);
    }
    return result;
}

expr parser::get_expr()
{
    expr node;
    if(!is_operand(true))
    {
        helper e;
        e.l = peek().loc;
        e.msg = "Invalid expression";
        e.type = helper_type::location_err;
        this->helpers.push_back(e);
        this->ok = false;
        return node;
    }

    // Get's the expression's range
    std::vector<operand> range;
    int op = this->cursor;
    this->cursor = op;
    int s = this->cursor;
    int e = -1;
    // Paren depth
    while(is_operand(false));

    e = this->cursor;
    this->cursor = s;

    while(this->cursor < e)
    {
        operand tmp = get_operand();
        if(tmp.type == optype::invalid){
            // Does an error twice now I guess
            helper e;
            e.l = peek().loc;
            e.msg = "Expression parsing failed";
            e.type = helper_type::line_err;
            this->helpers.push_back(e);
            this->ok = false;
            return node;
        }
        else
        {
            range.push_back(tmp);
        }
    }

    std::cout<<"==--- Expression Range Dump ---=="<<std::endl;
    for(operand o : range){
        std::cout<<o.dump(0)<<std::endl;
    }

    if(range.size() == 0){
        helper e;
        e.l = peek().loc;
        e.msg = "Cannot have empty expression";
        e.type = helper_type::line_err;
        this->ok = false;
        this->helpers.push_back(e);
    }
    else if(range.size() == 1){
        node.root = range.at(0);
    }
    else{
        operand o;
        o.type = optype::subexpr;
        o.node = std::make_shared<subexpr>(get_subexpr(range));
        node.root = o;
    }

    int c = 123;
    return node;
}