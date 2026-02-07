#include "formula.hpp"

bool truthtable(Formula* &f, std::set<std::string> &vars, Valuation &v){
    if(vars.size() == v.size()){
        return f->solve(v);
    }
    
    std::string current_var;
    for(auto var:vars){
        if(v.find(var) == v.end()){
            current_var = var;
            break;
        }
    }

    v[current_var] = true;
    auto sat = truthtable(f,vars,v);
    if(sat){
        v.erase(current_var);
        return true;
    }

    v[current_var] = false;
    sat = truthtable(f,vars,v);
    if(sat){
        v.erase(current_var);
        return true;
    }

    v.erase(current_var);
    return false;
}


bool sat_truthtable(Formula* &f){
    auto vars = f->get_vars();
    Valuation v;
    return truthtable(f,vars,v);
}