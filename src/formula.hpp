#ifndef __FORMULA__
#define __FORMULA__
#include <unordered_map>
#include <string>
#include <iostream>
#include <set>
#include <algorithm>

using Valuation = std::unordered_map<std::string, bool>;

struct Formula {
    virtual ~Formula() = default;
    virtual void print() const = 0;
    virtual bool solve(const Valuation& valuation) const = 0;
    virtual std::set<std::string> get_vars() const = 0;
};

struct Variable : Formula {
    std::string name;
    Variable(const std::string& n) : name(n) {}
    void print() const override;
    bool solve(const Valuation& valuation) const override;
    std::set<std::string> get_vars() const override;
};

struct Not : Formula {
    Formula* operand;

    explicit Not(Formula* op);
    ~Not() override;

    void print() const override;
    bool solve(const Valuation& valuation) const override;
    std::set<std::string> get_vars() const override;
};

struct BinaryOp : Formula {
    Formula* left;
    Formula* right;

    BinaryOp(Formula* l, Formula* r, char o);
    ~BinaryOp() override;

    std::set<std::string> get_vars() const override;
};

struct And : BinaryOp {
    And(Formula* l, Formula* r);

    void print() const override;
    bool solve(const Valuation& valuation) const override;
};

struct Or : BinaryOp {
    Or(Formula* l, Formula* r);

    void print() const override;
    bool solve(const Valuation& valuation) const override;
};

struct Implies : BinaryOp {
    Implies(Formula* l, Formula* r);

    void print() const override;
    bool solve(const Valuation& valuation) const override;
};

struct Eq : BinaryOp {
    Eq(Formula* l, Formula* r);

    void print() const override;
    bool solve(const Valuation& valuation) const override;
};





#endif