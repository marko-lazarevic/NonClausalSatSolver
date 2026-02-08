#include "formula.hpp"
#include "truthtable.hpp"
#include "dagformula.hpp"
#include "test_formulas.hpp"
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
    std::cout << std::endl << "signature: " << formula->signature() << std::endl;

    auto is_satisfiable = sat_truthtable(formula);

    auto str = is_satisfiable?"":"not ";
    std::cout << std::endl << "this formula is " << str << "satisfiable" << std::endl;

    // p & (~p)
    Formula* formula2 = new And(p,notp);

    formula2->print();
    std::cout << std::endl << "signature: " << formula2->signature() << std::endl;

    is_satisfiable = sat_truthtable(formula2);

    str = is_satisfiable?"":"not ";
    std::cout << std::endl << "this formula is " << str << "satisfiable" << std::endl;

    Formula* formula3 = new And(p, q);
    Formula* formula4 = new And(q, p);

    std::cout << "formula3 signature: " << formula3->signature() << std::endl;
    std::cout << "formula4 signature: " << formula4->signature() << std::endl;

    std::map<std::string, DAGNode*> node_map;

    auto dag = build_dag(formula, node_map, nullptr);
    dag->print();
    std::cout << std::endl;

    is_satisfiable = dag->label(TruthValue::TRUE, 0, TruthValueChangeReason::TRIGGER);
    str = is_satisfiable?"":"not ";
    std::cout << "After labeling the root as TRUE dag is " << str << "satisfiable" << std::endl;

    std::map<std::string, DAGNode*> node_map2;
    auto dag2 = build_dag(formula2, node_map2, nullptr);
    dag2->print();
    std::cout << std::endl;

    is_satisfiable = dag2->label(TruthValue::TRUE, 0, TruthValueChangeReason::TRIGGER);
    str = is_satisfiable?"":"not ";
    std::cout << "After labeling the root as TRUE dag2 is " << str << "satisfiable" << std::endl;


    std::cout<< "Testing formulas:" << std::endl;
    test_formulas();

} 