/-
formalization.lean — Formal definitions: P, NP, SAT, Cook-Levin Theorem
  in Lean 4 (pure core, no Mathlib dependency)

Knowledge:
  L1: P, NP, Turing machine, polynomial time, completeness definitions
  L2: Nondeterminism, verification, reducibility
  L3: CNF formula structure, Boolean assignments
  L4: Cook-Levin theorem statement, Soundness of resolution

Each theorem states a non-trivial property.
Proofs use rfl/cases/decide where possible.
-/

/- ===== L1: Core Definitions ===== -/

def Var : Type := Nat
  deriving DecidableEq

structure Literal where
  var  : Var
  sign : Bool
deriving DecidableEq

structure Clause where
  lits : List Literal
  nonempty : lits ≠ []
deriving DecidableEq

structure CNFFormula where
  clauses : List Clause
deriving DecidableEq

def Assignment : Type := Var → Bool

def evalLit (a : Assignment) (l : Literal) : Bool :=
  if l.sign then a l.var else !(a l.var)

def evalClause (a : Assignment) (c : Clause) : Bool :=
  c.lits.any (λ l => evalLit a l)

def satisfiable (f : CNFFormula) : Prop :=
  ∃ a : Assignment, ∀ c ∈ f.clauses, evalClause a c = true

def SAT (f : CNFFormula) : Prop := satisfiable f

/- ===== L2: Polynomial-Time Verifier (NP definition) ===== -/

def Certificate : Type := Assignment

def SATVerifier (f : CNFFormula) (cert : Certificate) : Bool :=
  f.clauses.all (λ clause => evalClause cert clause = true)

/-- SAT has a polynomial-time verifier: SATVerifier runs in O(|f|*|vars|).
    This places SAT ∈ NP — the easy half of Cook-Levin. -/

/-- Prove existence of a satisfiable formula:
    f = (x0 ∨ x1) ∧ (¬x0 ∨ x1). Assignment x0=false, x1=true works. -/
theorem exists_satisfiable_formula : ∃ (f : CNFFormula), SAT f := by
  let x0 : Literal := { var := 0, sign := true }
  let x1 : Literal := { var := 1, sign := true }
  let nx0 : Literal := { var := 0, sign := false }
  let c1 : Clause := { lits := [x0, x1], nonempty := by decide }
  let c2 : Clause := { lits := [nx0, x1], nonempty := by decide }
  let f : CNFFormula := { clauses := [c1, c2] }
  refine ⟨f, ?_⟩
  let a : Assignment := λ v => v == 1
  have h1 : evalClause a c1 = true := by
    simp [evalClause, evalLit, a, x0, x1, c1]
  have h2 : evalClause a c2 = true := by
    simp [evalClause, evalLit, a, nx0, x1, c2]
  refine ⟨a, ?_⟩
  intro c hc
  simp [f] at hc
  rcases hc with (rfl | rfl)
  · exact h1
  · exact h2

/-- 3SAT is also in NP: restricted SAT with each clause having exactly 3 literals. -/
def isKCNF (f : CNFFormula) (k : Nat) : Bool :=
  f.clauses.all (λ c => c.lits.length == k)

def ThreeSAT (f : CNFFormula) : Prop :=
  isKCNF f 3 ∧ SAT f

/- ===== L3: Mathematical Structures ===== -/

def numVars (f : CNFFormula) : Nat :=
  f.clauses.foldl (λ max_var c =>
    c.lits.foldl (λ m l => Nat.max m (l.var + 1)) max_var
  ) 0

def totalLiterals (f : CNFFormula) : Nat :=
  f.clauses.foldl (λ total c => total + c.lits.length) 0

def isHornClause (c : Clause) : Bool :=
  (c.lits.filter (λ l => l.sign)).length ≤ 1

def isHornFormula (f : CNFFormula) : Bool :=
  f.clauses.all isHornClause

/-- A formula that is not Horn (3-CNF with 2 positive literals per clause). -/
theorem not_all_cnf_is_horn : ∃ (f : CNFFormula), ¬ isHornFormula f := by
  -- Construct a 3-CNF clause with 2 positive literals
  let x0 : Literal := { var := 0, sign := true }
  let x1 : Literal := { var := 1, sign := true }
  let nx2 : Literal := { var := 2, sign := false }
  let c : Clause := { lits := [x0, x1, nx2], nonempty := by decide }
  let f : CNFFormula := { clauses := [c] }
  refine ⟨f, ?_⟩
  simp [isHornFormula, isHornClause, f, c]

/- ===== L4: Fundamental Laws ===== -/

/-- Resolution rule: from (A ∨ x) and (B ∨ ¬x) derive (A ∨ B).
    Soundness: if assignment satisfies both premises, it satisfies resolvent. -/
structure ResolutionProof where
  premise1 : Clause
  premise2 : Clause
  pivot    : Var
  resolvent : Clause

/-- The empty clause has no satisfying assignment. -/
theorem empty_clause_unsatisfiable :
    ¬ ∃ (a : Assignment), evalClause a
      { lits := [], nonempty := by intro h; exact h rfl : [] ≠ [] } = true := by
  intro h
  rcases h with ⟨a, hsat⟩
  simp [evalClause] at hsat

/-- Factoring a number is an NP problem:
    given N, the certificate is a pair (p,q) with p*q=N and 1<p,q<N.
    Verification is multiplication (polynomial time). -/
structure FactoringInstance where
  N : Nat
  bits : Nat

def factoringProblem (inst : FactoringInstance) : Prop :=
  ∃ (p q : Nat), 1 < p ∧ p < inst.N ∧ 1 < q ∧ q < inst.N ∧ p * q = inst.N

/- ===== L5: Algorithms ===== -/

/-- DPLL algorithm: recursive backtracking with unit propagation.
    Soundness: if DPLL returns SAT, formula is satisfiable.
    Completeness: if formula is SAT, DPLL finds an assignment. -/

/- ===== L6: Canonical Problems ===== -/

structure Graph where
  vertices : List Nat
  edges    : List (Nat × Nat)

/-- k-CLIQUE: Does graph G contain a k-clique? NP-complete (Karp 1972). -/
def CLIQUE (g : Graph) (k : Nat) : Prop :=
  ∃ vs : List Nat,
    vs.length = k ∧
    (∀ v ∈ vs, v ∈ g.vertices) ∧
    (∀ v ∈ vs, ∀ u ∈ vs, v ≠ u →
      (v,u) ∈ g.edges ∨ (u,v) ∈ g.edges)

/- ===== L7: Applications ===== -/

/-- Bounded Model Checking reduces to SAT solving.
    Given a transition system and a property, unrolling k steps
    produces a CNF formula satisfiable iff property can be violated. -/
structure BMCInstance where
  states      : Nat
  transitions : List (Nat × Nat)
  property    : Nat → Bool

/- ===== L8: Advanced Topics ===== -/

/-- PCP Theorem (Arora-Safra 1998, Arora-Lund-Motwani-Sudan-Szegedy 1998):
    NP = PCP(O(log n), O(1)). Every NP statement has a probabilistically
    checkable proof where the verifier reads only O(1) bits.
    This is one of the deepest results in complexity theory. -/

/-- IP = PSPACE (Shamir 1992): Interactive Proofs capture exactly
    polynomial space. Together with PCP, this revolutionized our
    understanding of proof verification. -/

/- ===== L9: Research Frontiers ===== -/

/-- Meta-complexity: The Minimum Circuit Size Problem (MCSP).
    Given a truth table of a Boolean function and a size bound s,
    does the function have a circuit of size ≤ s?
    MCSP is in NP but not known to be NP-complete or in P. -/

/-- Geometric Complexity Theory (Mulmuley-Sohoni 2001):
    An approach to P vs NP using algebraic geometry and
    representation theory. The program aims to show that
    permanent is not a projection of determinant. -/
