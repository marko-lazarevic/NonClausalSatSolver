#include "formula.hpp"
#include "truthtable.hpp"
#include "dagformula.hpp"
#include "test_formulas.hpp"
#include <iostream>
#include <cstring>

extern int yyparse();
extern Formula* parsed_formula;

int main(int argc, char* argv[]){
    // Check for -t flag
    if(argc > 1 && strcmp(argv[1], "-t") == 0) {
        std::cout << "Testing formulas:" << std::endl;
        test_formulas();
        return 0;
    }

    // Parse formula from stdin
    std::cout << "Enter formula (end with semicolon): ";
    yyparse();
    
    if(parsed_formula == nullptr) {
        std::cerr << "Failed to parse formula" << std::endl;
        return 1;
    }

    Formula* formula = parsed_formula;
    
    formula->print();
    std::cout << std::endl << "signature: " << formula->signature() << std::endl;

    auto is_satisfiable = sat_truthtable(formula);
    auto str = is_satisfiable?"":"not ";
    std::cout << std::endl << "this formula is " << str << "satisfiable" << std::endl;

    // Build DAG and label
    std::map<std::string, DAGNode*> node_map;
    auto dag = build_dag(formula, node_map, nullptr);
    dag->print();
    std::cout << std::endl;

    is_satisfiable = dag->label(TruthValue::TRUE, 0, TruthValueChangeReason::TRIGGER);
    str = is_satisfiable?"":"not ";
    std::cout << "After labeling the root as TRUE dag is " << str << "satisfiable" << std::endl;

    return 0;
} 