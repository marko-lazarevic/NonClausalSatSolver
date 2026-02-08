#include "formula.hpp"
#include "truthtable.hpp"
#include "dagformula.hpp"
#include <chrono>
#include <iomanip>
#include <map>
#include <string>
#include <vector>

namespace {
// Helper to avoid shared ownership; each call returns a fresh variable node.
Variable* v(const std::string& name) { return new Variable(name); }

Formula* disj3(Formula* a, Formula* b, Formula* c) {
	return new Or(new Or(a, b), c);
}

Formula* conj3(Formula* a, Formula* b, Formula* c) {
	return new And(new And(a, b), c);
}

Formula* and_all(const std::vector<Formula*>& parts) {
	if (parts.empty()) return nullptr;
	Formula* cur = parts[0];
	for (size_t i = 1; i < parts.size(); ++i) {
		cur = new And(cur, parts[i]);
	}
	return cur;
}

Formula* cnf_abc() {
	return conj3(
		new Or(v("A"), v("B")),
		new Or(new Not(v("A")), v("C")),
		new Or(new Not(v("B")), v("C"))
	);
}

Formula* cnf_def() {
	return conj3(
		new Or(v("D"), v("E")),
		new Or(new Not(v("D")), v("F")),
		new Or(new Not(v("E")), v("F"))
	);
}

Formula* implies_chain(const std::string& a, const std::string& b, const std::string& c) {
	return new And(new Implies(v(a), v(b)), new Implies(v(b), v(c)));
}

bool sat_dag(Formula* f) {
	std::map<std::string, DAGNode*> node_map;
	DAGNode* dag = build_dag(f, node_map, nullptr);
	return dag->label(TruthValue::TRUE, 0, TruthValueChangeReason::TRIGGER);
}

struct TestCase {
	std::string label;
	Formula* formula;
	bool expected_sat;
};

struct TestResult {
	bool tt_res;
	bool dag_res;
	double tt_secs;
	double dag_secs;
};

TestResult test_formula(Formula* f, const std::string& label, bool expected_sat) {
	std::cout << label << std::endl;
	f->print();
	std::cout << std::endl;
	std::cout << "expected: " << (expected_sat ? "SAT" : "UNSAT") << std::endl;

	auto tt_start = std::chrono::high_resolution_clock::now();
	bool tt_res = sat_truthtable(f);
	auto tt_end = std::chrono::high_resolution_clock::now();
	double tt_secs = std::chrono::duration_cast<std::chrono::duration<double>>(tt_end - tt_start).count();

	auto dag_start = std::chrono::high_resolution_clock::now();
	bool dag_res = sat_dag(f);
	auto dag_end = std::chrono::high_resolution_clock::now();
	double dag_secs = std::chrono::duration_cast<std::chrono::duration<double>>(dag_end - dag_start).count();

	std::cout << "- truthtable result: " << (tt_res ? "SAT" : "UNSAT") << std::endl;
	std::cout << "- truthtable time executed: " << std::fixed << std::setprecision(6) << tt_secs << " seconds" << std::endl;
	std::cout << "- dag result: " << (dag_res ? "SAT" : "UNSAT") << std::endl;
	std::cout << "- dag time executed: " << std::fixed << std::setprecision(6) << dag_secs << " seconds" << std::endl;
	std::cout << std::endl;

	return {tt_res, dag_res, tt_secs, dag_secs};
}
} // namespace

void test_formulas() {
	std::vector<TestCase> tests;

	tests.push_back({"A or not A", new Or(v("A"), new Not(v("A"))), true});
	tests.push_back({"p and not p", new And(v("p"), new Not(v("p"))), false});
	tests.push_back({"A implies B", new Implies(v("A"), v("B")), true});
	tests.push_back({"(X and Y) implies X", new Implies(new And(v("X"), v("Y")), v("X")), true});
	tests.push_back({"(not p => q) <=> (p or q)", new Eq(new Implies(new Not(v("p")), v("q")), new Or(v("p"), v("q"))), true});
	tests.push_back({"(A or B) and (not A or C) and (not B or C)", cnf_abc(), true});
	tests.push_back({"(P=>Q) and (Q=>R) and (not R)", new And(implies_chain("P", "Q", "R"), new Not(v("R"))), true});
	tests.push_back({"(X<=>Y) and (Y=>Z) and (not Z or X)", new And(new And(new Eq(v("X"), v("Y")), new Implies(v("Y"), v("Z"))), new Or(new Not(v("Z")), v("X"))), true});
	tests.push_back({"(A or B or C) and (not A or not B or C) and (not C or A)", conj3(disj3(v("A"), v("B"), v("C")), disj3(new Not(v("A")), new Not(v("B")), v("C")), new Or(new Not(v("C")), v("A"))), true});
	tests.push_back({"((P&Q)=>(R&S)) and (not R or not S)", new And(new Implies(new And(v("P"), v("Q")), new And(v("R"), v("S"))), new Or(new Not(v("R")), new Not(v("S")))), true});

	tests.push_back({"((A or B) and (not A or C) and (not B or C)) and ((D or E) and (not D or F) and (not E or F))", new And(cnf_abc(), cnf_def()), true});

	tests.push_back({"(P=>Q & Q=>R & not R) or (S=>T & T=>U & not U)", new Or(
		new And(implies_chain("P", "Q", "R"), new Not(v("R"))),
		new And(implies_chain("S", "T", "U"), new Not(v("U")))
	), true});

	tests.push_back({"(((A or B) and (not A or C)) and ((B or D) and (not B or E))) => (F and G)", new Implies(
		new And(new And(new Or(v("A"), v("B")), new Or(new Not(v("A")), v("C"))), new And(new Or(v("B"), v("D")), new Or(new Not(v("B")), v("E")))),
		new And(v("F"), v("G"))
	), true});

	tests.push_back({"(X<=>Y) & (Y=>Z) & (not Z or X) & (U=>V) & (V=>W) & (not W)", new And(
		new And(new And(new Eq(v("X"), v("Y")), new Implies(v("Y"), v("Z"))), new Or(new Not(v("Z")), v("X"))),
		new And(new And(new Implies(v("U"), v("V")), new Implies(v("V"), v("W"))), new Not(v("W")))
	), true});

	tests.push_back({"((A or B or C) and (not A or not B or C) and (not C or A) and (D or E) and (not D or F) and (not E or F)) or G",
		new Or(
			and_all({
				disj3(v("A"), v("B"), v("C")),
				disj3(new Not(v("A")), new Not(v("B")), v("C")),
				new Or(new Not(v("C")), v("A")),
				new Or(v("D"), v("E")),
				new Or(new Not(v("D")), v("F")),
				new Or(new Not(v("E")), v("F"))
			}),
			v("G")
		),
		true});

	tests.push_back({"((A or B) and (not A or C) and (not B or C)) and ((D or E) and (not D or F) and (not E or F)) and ((A or B) and (not A or C) and (not B or C))",
		new And(new And(cnf_abc(), cnf_def()), cnf_abc()),
		true});

	tests.push_back({"double UNSAT disjunction", new Or(
		new Or(new And(implies_chain("P", "Q", "R"), new Not(v("R"))), new And(implies_chain("S", "T", "U"), new Not(v("U")))),
		new And(implies_chain("P", "Q", "R"), new Not(v("R")))
	), true});

	tests.push_back({"(((A or B) and (not A or C)) and ((B or D) and (not B or E)) and ((A or B) and (not A or C))) => (F and G)",
		new Implies(
			and_all({
				new And(new Or(v("A"), v("B")), new Or(new Not(v("A")), v("C"))),
				new And(new Or(v("B"), v("D")), new Or(new Not(v("B")), v("E"))),
				new And(new Or(v("A"), v("B")), new Or(new Not(v("A")), v("C")))
			}),
			new And(v("F"), v("G"))
		),
		true});

	tests.push_back({"(X<=>Y) & (Y=>Z) & (not Z or X) & (U=>V) & (V=>W) & (not W) & (X<=>Y) & (Y=>Z)",
		and_all({
			new Eq(v("X"), v("Y")),
			new Implies(v("Y"), v("Z")),
			new Or(new Not(v("Z")), v("X")),
			new Implies(v("U"), v("V")),
			new Implies(v("V"), v("W")),
			new Not(v("W")),
			new Eq(v("X"), v("Y")),
			new Implies(v("Y"), v("Z"))
		}),
		true});

	tests.push_back({"((A or B or C) and (not A or not B or C) and (not C or A) and (D or E) and (not D or F) and (not E or F)) or ((A or B or C) and (not A or not B or C))",
		new Or(
			and_all({
				disj3(v("A"), v("B"), v("C")),
				disj3(new Not(v("A")), new Not(v("B")), v("C")),
				new Or(new Not(v("C")), v("A")),
				new Or(v("D"), v("E")),
				new Or(new Not(v("D")), v("F")),
				new Or(new Not(v("E")), v("F"))
			}),
			and_all({
				disj3(v("A"), v("B"), v("C")),
				disj3(new Not(v("A")), new Not(v("B")), v("C"))
			})
		),
		true});

	tests.push_back({"((p and q) => r) and ((p and q) => s) and ((p and q) => t) and (p and q)",
		and_all({
			new Implies(new And(v("p"), v("q")), v("r")),
			new Implies(new And(v("p"), v("q")), v("s")),
			new Implies(new And(v("p"), v("q")), v("t")),
			new And(v("p"), v("q"))
		}),
		true});

	tests.push_back({"(a => b) and (b => c) and (c => d) and (d => e) and (e => not a) and a",
		and_all({
			new Implies(v("a"), v("b")),
			new Implies(v("b"), v("c")),
			new Implies(v("c"), v("d")),
			new Implies(v("d"), v("e")),
			new Implies(v("e"), new Not(v("a"))),
			v("a")
		}),
		false});

	auto triple = [](){ return new And(new And(v("x1"), v("x2")), v("x3")); };
	Formula* repeated_triple = new Or(new Or(new Or(new Or(triple(), triple()), triple()), triple()), triple());
	tests.push_back({"(x1 and x2 and x3) repeated disjunction", repeated_triple, true});

	tests.push_back({"(p or q) and (p or r) and (p or s) and (p or t) and not p",
		and_all({
			new Or(v("p"), v("q")),
			new Or(v("p"), v("r")),
			new Or(v("p"), v("s")),
			new Or(v("p"), v("t")),
			new Not(v("p"))
		}),
		true});


	int tt_correct = 0;
	int tt_incorrect = 0;
	int dag_correct = 0;
	int dag_incorrect = 0;
	int dag_faster = 0;
	int dag_slower = 0;

	for (const auto& t : tests) {
		TestResult r = test_formula(t.formula, t.label, t.expected_sat);
		if (r.tt_res == t.expected_sat) {
			++tt_correct;
		} else {
			++tt_incorrect;
		}
		if (r.dag_res == t.expected_sat) {
			++dag_correct;
		} else {
			++dag_incorrect;
		}
		if (r.dag_secs < r.tt_secs) {
			++dag_faster;
		} else if (r.dag_secs > r.tt_secs) {
			++dag_slower;
		}
	}

	std::cout << "Summary:" << std::endl;
	std::cout << "- truthtable correct: " << tt_correct << " | incorrect: " << tt_incorrect << std::endl;
	std::cout << "- dag correct: " << dag_correct << " | incorrect: " << dag_incorrect << std::endl;
	std::cout << "- dag faster: " << dag_faster << " | dag slower: " << dag_slower << std::endl;
}