# Non-clausal SAT solver

“Non-clausal SAT solver” is a seminar paper for the Master’s course **Automated Reasoning** at the Faculty of Mathematics, University of Belgrade.

Author: Marko Lazarević

Main goal of this work is to implement a basic SAT solver that will work with formulas in general form rather than CNF which is widely adopted among all industry-standard SAT solvers.

## SAT problem

In logic and computer science, the Boolean satisfiability problem (sometimes called propositional satisfiability problem and abbreviated SATISFIABILITY, SAT or B-SAT) asks whether there exists an interpretation that satisfies a given Boolean formula. In other words, it asks whether the formula's variables can be consistently replaced by the values TRUE or FALSE to make the formula evaluate to TRUE. If this is the case, the formula is called satisfiable, else unsatisfiable. For example, the formula "a AND NOT b" is satisfiable because one can find the values a = TRUE and b = FALSE, which make (a AND NOT b) = TRUE. In contrast, "a AND NOT a" is unsatisfiable.

SAT is the first problem that was proven to be NP-complete—this is the Cook–Levin theorem. This means that all problems in the complexity class NP, which includes a wide range of natural decision and optimization problems, are at most as difficult to solve as SAT. There is no known algorithm that efficiently solves each SAT problem (where "efficiently" means "deterministically in polynomial time"). Although such an algorithm is generally believed not to exist, this belief has not been proven or disproven mathematically. Resolving the question of whether SAT has a polynomial-time algorithm would settle the P versus NP problem - one of the most important open problems in the theory of computing.

## Truth table approach

Truth table approach is used as a reference to check correctness of the DAG-based approach. It uses a basic brute-force algorithm that will try all possible valuations for variables from the given formula; the first time we find a valuation that satisfies the formula we return that the formula is satisfiable. This approach works great for formulas with a small number of variables.

## Labeling Rules for DAG-based Non-Clausal SAT Solving

Each node in the DAG can be in one of three states: **TRUE**, **FALSE**, or **UNLABELED**.  
Propagation rules are applied both **top-down** (from parent to children) and **bottom-up** (from children to parent).  
A conflict occurs if a node is forced to take two different truth values.

---

### NOT (¬)

1. If the NOT node becomes **TRUE**, propagate **FALSE** to its child.
2. If the NOT node becomes **FALSE**, propagate **TRUE** to its child.
3. If the child becomes **TRUE**, propagate **FALSE** to the NOT node.
4. If the child becomes **FALSE**, propagate **TRUE** to the NOT node.

---

### AND (∧)

1. If the AND node becomes **TRUE**, propagate **TRUE** to all of its children.
2. If the AND node is **FALSE** and all but one child are **TRUE**, propagate **FALSE** to the remaining unlabeled child.
3. If any child becomes **FALSE**, propagate **FALSE** to the AND node.
4. If all children become **TRUE**, propagate **TRUE** to the AND node.

---

### OR (∨)

1. If the OR node becomes **FALSE**, propagate **FALSE** to all of its children.
2. If the OR node is **TRUE** and all but one child are **FALSE**, propagate **TRUE** to the remaining unlabeled child.
3. If any child becomes **TRUE**, propagate **TRUE** to the OR node.
4. If all children become **FALSE**, propagate **FALSE** to the OR node.

---

### IMPLIES (⇒)

Let the implication be `A ⇒ B`.

1. If the IMPLIES node becomes **FALSE**, propagate **TRUE** to `A` and **FALSE** to `B`.
2. If `A` becomes **FALSE**, propagate **TRUE** to the IMPLIES node.
3. If `B` becomes **TRUE**, propagate **TRUE** to the IMPLIES node.
4. If the IMPLIES node is **TRUE** and `A` is **TRUE**, propagate **TRUE** to `B`.
5. If the IMPLIES node is **TRUE** and `B` is **FALSE**, propagate **FALSE** to `A`.
6. If `A` becomes **TRUE** and consequent **FALSE**, propagate **FALSE** to the IMPLIES node.
7. If the IMPLIES node is **TRUE** and `A` later becomes **TRUE**, propagate **TRUE** to `B`.
8. If the IMPLIES node is **TRUE** and `B` later becomes **FALSE**, propagate **FALSE** to `A`.

---

### EQUIVALENCE (⇔)

Let the equivalence be `A ⇔ B`.

1. If the EQUIVALENCE node becomes **TRUE**, propagate the same truth value to both `A` and `B`.
2. If the EQUIVALENCE node becomes **FALSE**, propagate opposite truth values to `A` and `B`.
3. If `A` and `B` become **TRUE**, propagate **TRUE** to the EQUIVALENCE node.
4. If `A` and `B` become **FALSE**, propagate **TRUE** to the EQUIVALENCE node.
5. If one of `A` or `B` is **TRUE** and the other is **FALSE**, propagate **FALSE** to the EQUIVALENCE node.
6. If the EQUIVALENCE node is **TRUE** and one side is labeled, propagate the same value to the other side.
7. If the EQUIVALENCE node is **FALSE** and one side is labeled, propagate the opposite value to the other side.

---

### Conflict Detection

A conflict is detected if:

- a node is forced to be both **TRUE** and **FALSE**

In case of conflict, the current branch is rejected and the solver backtracks.

## Benchmark formulas

| Formula                                                                                                    | Satisfiable |
| ---------------------------------------------------------------------------------------------------------- | ----------- |
| `A ∨ ¬A`                                                                                                   | SAT         |
| `p ∧ ¬p`                                                                                                   | UNSAT       |
| `(p ∨ q) ∧ ¬p ∧ ¬q`                                                                                        | UNSAT       |
| `¬(p ∨ ¬p)`                                                                                                | UNSAT       |
| `(p ⇒ q) ∧ p ∧ ¬q`                                                                                         | UNSAT       |
| `(p ⇔ q) ∧ p ∧ ¬q`                                                                                         | UNSAT       |
| `A ⇒ B`                                                                                                    | SAT         |
| `(X ∧ Y) ⇒ X`                                                                                              | SAT         |
| `(¬p ⇒ q) ⇔ (p ∨ q)`                                                                                       | SAT         |
| `(A ∨ B) ∧ (¬A ∨ C) ∧ (¬B ∨ C)`                                                                            | SAT         |
| `(P ⇒ Q) ∧ (Q ⇒ R) ∧ (¬R)`                                                                                 | SAT         |
| `(X ⇔ Y) ∧ (Y ⇒ Z) ∧ (¬Z ∨ X)`                                                                             | SAT         |
| `(A ∨ B ∨ C) ∧ (¬A ∨ ¬B ∨ C) ∧ (¬C ∨ A)`                                                                   | SAT         |
| `(P ∧ Q) ⇒ (R ∧ S) ∧ (¬R ∨ ¬S)`                                                                            | SAT         |
| `((A ∨ B) ∧ (¬A ∨ C) ∧ (¬B ∨ C)) ∧ ((D ∨ E) ∧ (¬D ∨ F) ∧ (¬E ∨ F))`                                        | SAT         |
| `((P ⇒ Q) ∧ (Q ⇒ R) ∧ (¬R)) ∨ ((S ⇒ T) ∧ (T ⇒ U) ∧ ¬U)`                                                    | SAT         |
| `(((A ∨ B) ∧ (¬A ∨ C)) ∧ ((B ∨ D) ∧ (¬B ∨ E))) ⇒ (F ∧ G)`                                                  | SAT         |
| `(X ⇔ Y) ∧ (Y ⇒ Z) ∧ (¬Z ∨ X) ∧ (U ⇒ V) ∧ (V ⇒ W) ∧ (¬W)`                                                  | SAT         |
| `((A ∨ B ∨ C) ∧ (¬A ∨ ¬B ∨ C) ∧ (¬C ∨ A) ∧ (D ∨ E) ∧ (¬D ∨ F) ∧ (¬E ∨ F)) ∨ G`                             | SAT         |
| `((A ∨ B) ∧ (¬A ∨ C) ∧ (¬B ∨ C)) ∧ ((D ∨ E) ∧ (¬D ∨ F) ∧ (¬E ∨ F)) ∧ ((A ∨ B) ∧ (¬A ∨ C) ∧ (¬B ∨ C))`      | SAT         |
| `((P ⇒ Q) ∧ (Q ⇒ R) ∧ (¬R)) ∨ ((S ⇒ T) ∧ (T ⇒ U) ∧ ¬U) ∨ ((P ⇒ Q) ∧ (Q ⇒ R) ∧ (¬R))`                       | SAT         |
| `(((A ∨ B) ∧ (¬A ∨ C)) ∧ ((B ∨ D) ∧ (¬B ∨ E)) ∧ ((A ∨ B) ∧ (¬A ∨ C))) ⇒ (F ∧ G)`                           | SAT         |
| `(X ⇔ Y) ∧ (Y ⇒ Z) ∧ (¬Z ∨ X) ∧ (U ⇒ V) ∧ (V ⇒ W) ∧ (¬W) ∧ (X ⇔ Y) ∧ (Y ⇒ Z)`                              | SAT         |
| `((A ∨ B ∨ C) ∧ (¬A ∨ ¬B ∨ C) ∧ (¬C ∨ A) ∧ (D ∨ E) ∧ (¬D ∨ F) ∧ (¬E ∨ F)) ∨ ((A ∨ B ∨ C) ∧ (¬A ∨ ¬B ∨ C))` | SAT         |
| `((p ∧ q) ⇒ r) ∧ ((p ∧ q) ⇒ s) ∧ ((p ∧ q) ⇒ t) ∧ (p ∧ q)`                                                  | SAT         |
| `(a ⇒ b) ∧ (b ⇒ c) ∧ (c ⇒ d) ∧ (d ⇒ e) ∧ (e ⇒ ¬a) ∧ a`                                                     | UNSAT       |
| `(x1 ∧ x2 ∧ x3) ∨ (x1 ∧ x2 ∧ x3) ∨ (x1 ∧ x2 ∧ x3) ∨ (x1 ∧ x2 ∧ x3) ∨ (x1 ∧ x2 ∧ x3)`                       | SAT         |
| `((p ∨ q) ∧ (p ∨ r) ∧ (p ∨ s) ∧ (p ∨ t) ∧ ¬p)`                                                             | SAT         |
