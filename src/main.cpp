#include "formula.hpp"
#include "truthtable.hpp"
#include <iostream>

int main(){
    // (~p => q) <=> (p v q)
    auto p = new Variable("p");
    auto q = new Variable("q");
    auto notp = new Not(p);
    auto notpq = new Implies(notp,q);
    auto porq = new Or(p,q);
    Formula* formula = new Eq(notpq, porq);

    formula->print();

    auto is_satisfiable = sat_truthtable(formula);

    auto str = is_satisfiable?"":"not ";
    std::cout << std::endl << "this formula is " << str << "satisfiable" << std::endl;

    // p & (~p)
    Formula* formula2 = new And(p,notp);

    formula2->print();

    is_satisfiable = sat_truthtable(formula2);

    str = is_satisfiable?"":"not ";
    std::cout << std::endl << "this formula is " << str << "satisfiable" << std::endl;
} 