#include "formula.hpp"
#include <iterator>

void Variable::print() const {
    std::cout << name;
}

bool Variable::solve(const Valuation& valuation) const {
    auto it = valuation.find(name);
    return it != valuation.end() ? it->second : false;
}

std::set<std::string> Variable::get_vars() const {
    return {name};
}

Not::Not(Formula* op) : operand(op) {}

Not::~Not() {
    delete operand;
}

void Not::print() const {
    std::cout << "¬";
    operand->print();
}

bool Not::solve(const Valuation& valuation) const {
    return !operand->solve(valuation);
}

std::set<std::string> Not::get_vars() const {
    return operand->get_vars();
}

BinaryOp::BinaryOp(Formula* l, Formula* r, char) : left(l), right(r) {}

BinaryOp::~BinaryOp() {
    delete left;
    delete right;
}

std::set<std::string> BinaryOp::get_vars() const {
    auto left_vars = left->get_vars();
    auto right_vars = right->get_vars();
    std::set<std::string> res;
    std::set_union(
        left_vars.begin(), left_vars.end(),
        right_vars.begin(), right_vars.end(),
        std::inserter(res, res.begin()));
    return res;
}

And::And(Formula* l, Formula* r) : BinaryOp(l, r, '&') {}

void And::print() const {
    std::cout << "(";
    left->print();
    std::cout << " ∧ ";
    right->print();
    std::cout << ")";
}

bool And::solve(const Valuation& valuation) const {
    return left->solve(valuation) && right->solve(valuation);
}

std::string And::signature() const {
    auto left_sig = left->signature();
    auto right_sig = right->signature();

    //in order to have a unique signature for logically equivalent formulas, 
    //we sort the signatures of the left and right subformulas
    if(left_sig < right_sig){
        return "AND(" + left_sig + "," + right_sig + ")";
    }
    return "AND(" + right_sig + "," + left_sig + ")";
}

Or::Or(Formula* l, Formula* r) : BinaryOp(l, r, '|') {}

void Or::print() const {
    std::cout << "(";
    left->print();
    std::cout << " ∨ ";
    right->print();
    std::cout << ")";
}

bool Or::solve(const Valuation& valuation) const {
    return left->solve(valuation) || right->solve(valuation);
}

std::string Or::signature() const {
    auto left_sig = left->signature();
    auto right_sig = right->signature();

    //in order to have a unique signature for logically equivalent formulas, 
    //we sort the signatures of the left and right subformulas
    if(left_sig < right_sig){
        return "OR(" + left_sig + "," + right_sig + ")";
    }
    return "OR(" + right_sig + "," + left_sig + ")";
}

Implies::Implies(Formula* l, Formula* r) : BinaryOp(l, r, '>') {}

void Implies::print() const {
    std::cout << "(";
    left->print();
    std::cout << " => ";
    right->print();
    std::cout << ")";
}

bool Implies::solve(const Valuation& valuation) const {
    return !left->solve(valuation) || right->solve(valuation);
}

std::string Implies::signature() const {
    return "IMP(" + left->signature() + "," + right->signature() + ")";
}

Eq::Eq(Formula* l, Formula* r) : BinaryOp(l, r, '=') {}

void Eq::print() const {
    std::cout << "(";
    left->print();
    std::cout << " <=> ";
    right->print();
    std::cout << ")";
}

bool Eq::solve(const Valuation& valuation) const {
    return left->solve(valuation) == right->solve(valuation);
}

std::string Eq::signature() const {
    auto left_sig = left->signature();
    auto right_sig = right->signature();

    //in order to have a unique signature for logically equivalent formulas, 
    //we sort the signatures of the left and right subformulas
    if(left_sig < right_sig){
        return "EQ(" + left_sig + "," + right_sig + ")";
    }
    return "EQ(" + right_sig + "," + left_sig + ")";
}

