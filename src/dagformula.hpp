#ifndef __DAG_FORMULA__
#define __DAG_FORMULA__
#include "formula.hpp"
#include <vector>

enum class DAGOp { VAR, NOT, AND, OR, IMPLIES, EQ };
enum class TruthValue { TRUE, FALSE, UNKNOWN, DONTCARE };
enum class TruthValueChangeReason { TRIGGER, CONFLICT};

struct TruthValueChange {
    TruthValue old_value;
    int level;
    TruthValueChangeReason reason;

    TruthValueChange(TruthValue old_val, int lvl, TruthValueChangeReason r)
        : old_value(old_val), level(lvl), reason(r) {}
};

struct DAGNode {
    DAGOp op;
    std::string var;  // only used if op == VAR
    std::vector<DAGNode*> parents;
    std::vector<DAGNode*> children;
    TruthValue truth_value;
    TruthValueChange last_change;
    DAGNode() : truth_value(TruthValue::UNKNOWN), last_change(TruthValue::UNKNOWN, 0, TruthValueChangeReason::TRIGGER) {}

    // try to lable this node with new_value at the given level, 
    // return true if successful, false if it leads to a conflict
    bool label(TruthValue new_value, int level, TruthValueChangeReason reason);
    bool propagate_children(TruthValue new_value, int level);
    bool propagate_parents(TruthValue new_value, int level);

    void print() const;
};

DAGNode* build_dag(Formula *f, std::map<std::string, DAGNode*>& node_map, DAGNode* parent = nullptr);

#endif