#include "dagformula.hpp"

DAGNode* build_dag(Formula *f, std::map<std::string, DAGNode*>& node_map, DAGNode* parent) {
    auto sig = f->signature();
    if(node_map.count(sig)){
        auto node = node_map[sig];
        if(parent){
            node->parents.push_back(parent);
        }
        return node;
    }

    DAGNode* node = new DAGNode();

    if(parent){
        node->parents.push_back(parent);
    }

    if (auto var = dynamic_cast<Variable*>(f)) {
        node->op = DAGOp::VAR;
        node->var = var->name;
    }
    else if (auto not_op = dynamic_cast<Not*>(f)) {
        node->op = DAGOp::NOT;
        node->children.push_back(
            build_dag(not_op->operand, node_map, node)
        );
    }
    else if (auto and_op = dynamic_cast<And*>(f)) {
        node->op = DAGOp::AND;
        node->children.push_back(build_dag(and_op->left, node_map, node));
        node->children.push_back(build_dag(and_op->right, node_map, node));
    }
    else if (auto or_op = dynamic_cast<Or*>(f)) {
        node->op = DAGOp::OR;
        node->children.push_back(build_dag(or_op->left, node_map, node));
        node->children.push_back(build_dag(or_op->right, node_map, node));
    }
    else if (auto impl = dynamic_cast<Implies*>(f)) {
        node->op = DAGOp::IMPLIES;
        node->children.push_back(build_dag(impl->left, node_map, node));
        node->children.push_back(build_dag(impl->right, node_map, node));
    }
    else if (auto eq = dynamic_cast<Eq*>(f)) {
        node->op = DAGOp::EQ;
        node->children.push_back(build_dag(eq->left, node_map, node));
        node->children.push_back(build_dag(eq->right, node_map, node));
    }

    node_map[sig] = node;
    return node;
}

void DAGNode::print() const {
    switch (op) {
        case DAGOp::VAR:
            std::cout << var;
            break;
        case DAGOp::NOT:
            std::cout << "NOT(";
            children[0]->print();
            std::cout << ")";
            break;
        case DAGOp::AND:
            std::cout << "AND(";
            children[0]->print();
            std::cout << ",";
            children[1]->print();
            std::cout << ")";
            break;
        case DAGOp::OR:
            std::cout << "OR(";
            children[0]->print();
            std::cout << ",";
            children[1]->print();
            std::cout << ")";
            break;
        case DAGOp::IMPLIES:
            std::cout << "IMP(";
            children[0]->print();
            std::cout << ",";
            children[1]->print();
            std::cout << ")";
            break;
        case DAGOp::EQ:
            std::cout << "EQ(";
            children[0]->print();
            std::cout << ",";
            children[1]->print();
            std::cout << ")";
            break;
    }

}

bool DAGNode::propagate_children(TruthValue new_value, int level) {
    switch (op)
    {
        case DAGOp::VAR:
        // Variable nodes have no children to propagate to, so we can just return true.
        return true;
            break;
        case DAGOp::NOT:

        //If the NOT node becomes TRUE, propagate FALSE to its child.
        if(new_value == TruthValue::TRUE){
            if(!children[0]->label(TruthValue::FALSE, level+1, TruthValueChangeReason::TRIGGER)){
                return false;
            }
        }
        //If the NOT node becomes FALSE, propagate TRUE to its child.
        if(new_value == TruthValue::FALSE){
            if(!children[0]->label(TruthValue::TRUE, level+1, TruthValueChangeReason::TRIGGER)){
                return false;
            }
        }
            break;
        case DAGOp::AND:
         //  If the AND node becomes TRUE, propagate TRUE to all of its children.
        if(new_value == TruthValue::TRUE){
            for(auto child: children){
                if(!child->label(TruthValue::TRUE, level+1, TruthValueChangeReason::TRIGGER)){
                    return false;
                }
            }
        }
        // If the AND node is FALSE and all but one child are TRUE, propagate FALSE to the remaining unlabeled 
        if(new_value == TruthValue::FALSE){
            DAGNode* unlabeled_child = nullptr;
            bool more_than_one_unlabeled = false;
            for(auto child: children){
                if(child->truth_value == TruthValue::UNKNOWN){
                    if(unlabeled_child == nullptr){
                        unlabeled_child = child;
                    } else {
                        more_than_one_unlabeled = true;
                    }
                }
            }

            if(unlabeled_child && !more_than_one_unlabeled){
                if(!unlabeled_child->label(TruthValue::FALSE, level+1, TruthValueChangeReason::TRIGGER)){
                    return false;
                }
            }
        }
            break;
        case DAGOp::OR:
            // If the OR node becomes FALSE, propagate FALSE to all children.
            if(new_value == TruthValue::FALSE){
                for(auto child: children){
                    if(!child->label(TruthValue::FALSE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                }
            }
            // If the OR node is TRUE and all but one child are FALSE, propagate TRUE to the remaining unlabeled child.
            if(new_value == TruthValue::TRUE){
                DAGNode* unlabeled_child = nullptr;
                bool more_than_one_unlabeled = false;
                bool any_non_false = false;
                for(auto child: children){
                    if(child->truth_value == TruthValue::UNKNOWN){
                        if(unlabeled_child == nullptr){
                            unlabeled_child = child;
                        } else {
                            more_than_one_unlabeled = true;
                        }
                    } else if(child->truth_value != TruthValue::FALSE){
                        any_non_false = true;
                    }
                }

                if(!any_non_false && unlabeled_child && !more_than_one_unlabeled){
                    if(!unlabeled_child->label(TruthValue::TRUE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                }
            }
            break;
        case DAGOp::IMPLIES:
            // Let A = children[0], B = children[1]
            if(new_value == TruthValue::FALSE){
                if(!children[0]->label(TruthValue::TRUE, level+1, TruthValueChangeReason::TRIGGER)){
                    return false;
                }
                if(!children[1]->label(TruthValue::FALSE, level+1, TruthValueChangeReason::TRIGGER)){
                    return false;
                }
            }
            if(new_value == TruthValue::TRUE){
                if(children[0]->truth_value == TruthValue::TRUE){
                    if(!children[1]->label(TruthValue::TRUE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                }
                if(children[1]->truth_value == TruthValue::FALSE){
                    if(!children[0]->label(TruthValue::FALSE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                }
            }
            break;
        case DAGOp::EQ:
            if(new_value == TruthValue::TRUE){
                // If one side is known, force the other to the same value.
                if(children[0]->truth_value == TruthValue::TRUE){
                    if(!children[1]->label(TruthValue::TRUE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                } else if(children[0]->truth_value == TruthValue::FALSE){
                    if(!children[1]->label(TruthValue::FALSE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                }

                if(children[1]->truth_value == TruthValue::TRUE){
                    if(!children[0]->label(TruthValue::TRUE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                } else if(children[1]->truth_value == TruthValue::FALSE){
                    if(!children[0]->label(TruthValue::FALSE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                }
            }
            if(new_value == TruthValue::FALSE){
                // If one side is known, force the other to the opposite value.
                if(children[0]->truth_value == TruthValue::TRUE){
                    if(!children[1]->label(TruthValue::FALSE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                } else if(children[0]->truth_value == TruthValue::FALSE){
                    if(!children[1]->label(TruthValue::TRUE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                }

                if(children[1]->truth_value == TruthValue::TRUE){
                    if(!children[0]->label(TruthValue::FALSE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                } else if(children[1]->truth_value == TruthValue::FALSE){
                    if(!children[0]->label(TruthValue::TRUE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                }
            }
            break;
    };
    return true;
}

bool DAGNode::propagate_parents(TruthValue new_value, int level) {
    for(auto parent:parents){
        switch(parent->op){
            case DAGOp::NOT:
                if(new_value == TruthValue::TRUE){
                    if(!parent->label(TruthValue::FALSE,level+1,TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                }

                if(new_value == TruthValue::FALSE){
                    if(!parent->label(TruthValue::TRUE,level+1,TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                }
            break;

            case DAGOp::AND: {
                // If any child becomes FALSE, the AND becomes FALSE (rule 3 bottom-up).
                if(new_value == TruthValue::FALSE){
                    if(!parent->label(TruthValue::FALSE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                }
                // If all children are TRUE, the AND becomes TRUE (rule 4 bottom-up).
                else if(new_value == TruthValue::TRUE){
                    bool all_true = true;
                    for(auto sib : parent->children){
                        if(sib->truth_value != TruthValue::TRUE){
                            all_true = false;
                            break;
                        }
                    }
                    if(all_true){
                        if(!parent->label(TruthValue::TRUE, level+1, TruthValueChangeReason::TRIGGER)){
                            return false;
                        }
                    }
                }
            }
            break;

            case DAGOp::OR: {
                // If any child becomes TRUE, the OR becomes TRUE (rule 3 bottom-up).
                if(new_value == TruthValue::TRUE){
                    if(!parent->label(TruthValue::TRUE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                }
                // If all children are FALSE, the OR becomes FALSE (rule 4 bottom-up).
                else if(new_value == TruthValue::FALSE){
                    bool all_false = true;
                    for(auto sib : parent->children){
                        if(sib->truth_value != TruthValue::FALSE){
                            all_false = false;
                            break;
                        }
                    }
                    if(all_false){
                        if(!parent->label(TruthValue::FALSE, level+1, TruthValueChangeReason::TRIGGER)){
                            return false;
                        }
                    }
                }
            }
            break;

            case DAGOp::IMPLIES: {
                DAGNode* a = parent->children[0];
                DAGNode* b = parent->children[1];

                // If antecedent is FALSE -> implication TRUE (rule 2 bottom-up).
                if(a->truth_value == TruthValue::FALSE){
                    if(!parent->label(TruthValue::TRUE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                    break;
                }
                // If consequent is TRUE -> implication TRUE (rule 3 bottom-up).
                if(b->truth_value == TruthValue::TRUE){
                    if(!parent->label(TruthValue::TRUE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                    break;
                }
                // If antecedent TRUE and consequent FALSE -> implication FALSE (truth-table).
                if(a->truth_value == TruthValue::TRUE && b->truth_value == TruthValue::FALSE){
                    if(!parent->label(TruthValue::FALSE, level+1, TruthValueChangeReason::TRIGGER)){
                        return false;
                    }
                }
            }
            break;

            case DAGOp::EQ: {
                auto left_val = parent->children[0]->truth_value;
                auto right_val = parent->children[1]->truth_value;

                if(left_val != TruthValue::UNKNOWN && right_val != TruthValue::UNKNOWN){
                    if(left_val == right_val){
                        if(!parent->label(TruthValue::TRUE, level+1, TruthValueChangeReason::TRIGGER)){
                            return false;
                        }
                    } else {
                        if(!parent->label(TruthValue::FALSE, level+1, TruthValueChangeReason::TRIGGER)){
                            return false;
                        }
                    }
                }
            }
            break;
        }
    }
    return true;
}

bool DAGNode::label(TruthValue new_value, int level, TruthValueChangeReason reason) {
    if(truth_value == TruthValue::TRUE && new_value == TruthValue::FALSE || truth_value == TruthValue::FALSE && new_value == TruthValue::TRUE){
        return false; // conflict
    }

    if(truth_value == new_value){
        return true; // already labeled with the same value
    }

    auto old_value = truth_value;
    auto old_change = last_change;
    last_change = TruthValueChange(truth_value, level, reason);
    truth_value = new_value;

    bool can_propagate_children = this->propagate_children(new_value, level);
    if(!can_propagate_children){
        truth_value = old_value;
        last_change = old_change;
        return false; // conflict during propagation to children
    }
    bool can_propagate_parents = this->propagate_parents(new_value, level);
    if(!can_propagate_parents){
        truth_value = old_value;
        last_change = old_change;
        return false; // conflict during propagation to parents
    }

    return true; // we successfully labeled this node with new_value
}