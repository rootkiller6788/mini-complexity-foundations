/* ph_demo.c - PH Module: All Demonstrations and Structural Functions */
#include "ph.h"
#include <string.h>

/* ================================================================
 * Hierarchy Printing
 * ================================================================ */
void ph_print_hierarchy(void) {
    printf("\n========== The Polynomial Hierarchy ==========\n\n");
    printf("  Level   Class        Canonical Complete Problem\n");
    printf("  -----   -----        -------------------------\n");
    printf("  0       P=Sigma_0    Circuit Value Problem\n");
    printf("  1       NP=Sigma_1   SAT\n");
    printf("  1       coNP=Pi_1    UNSAT / TAUTOLOGY\n");
    printf("  2       Sigma_2      QSAT_2 (exists x forall y: phi)\n");
    printf("  2       Pi_2         co-QSAT_2 (forall x exists y: phi)\n");
    printf("  3       Sigma_3      QSAT_3\n");
    printf("  4       Sigma_4      QSAT_4\n");
    printf("  ...\n");
    printf("  PH = U_k Sigma_k = U_k Pi_k\n\n");
    printf("  Theorem (Meyer-Stockmeyer): Sigma_k-SAT is complete for Sigma_k\n");
    printf("  Theorem (Toda 1991): PH subseteq P^{#P} (Godel Prize 1998)\n");
    printf("  Theorem (Karp-Lipton 1982): NP in P/poly => PH = Sigma_2\n");
    printf("  Theorem (SGL 1983): BPP subseteq Sigma_2 cap Pi_2\n");
    printf("  Theorem (CKS 1981): Sigma_k = ATIME(poly, k alternations)\n");
    printf("  Open: Is PH infinite? (implies P != NP)\n");
}

/* ================================================================
 * Module Summary
 * ================================================================ */
void ph_summary(void) {
    printf("\n===== PH Module Summary =====\n\n");
    printf("Sigma_0 = P. Sigma_{k+1} = NP^{Sigma_k}. Pi_k = co-Sigma_k.\n");
    printf("PH = U_k Sigma_k = U_k Pi_k.\n\n");
    printf("Core Theorems Implemented:\n");
    printf("  L4.1 Sigma_k-SAT is Sigma_k-complete (Stockmeyer 1976)\n");
    printf("  L4.2 Sigma_k=Pi_k => PH=Sigma_k (collapse)\n");
    printf("  L4.3 NP in P/poly => PH=Sigma_2 (Karp-Lipton 1982)\n");
    printf("  L4.4 PH subseteq P^{#P} (Toda 1991, Godel Prize 1998)\n");
    printf("  L4.5 BPP subseteq Sigma_2 cap Pi_2 (SGL 1983)\n");
    printf("  L4.6 Sigma_k = ATIME(poly, k alts) (CKS 1981)\n\n");
    printf("L5 Algorithms: DPLL SAT, SAT-to-CLIQUE, SAT-to-VERTEX-COVER\n");
    printf("L6 Complete Problems: QSAT_k, TQBF, Circuit Value\n");
    printf("L7 Applications: Bounded Model Checking, Game Solving, Circuit Min\n");
    printf("L8 Advanced: PCP verification, Valiant-Vazirani isolation\n");
    printf("L9 Research: Meta-complexity, GCT, quantum complexity, PH infinite?\n\n");
    printf("Big open problem: Is the polynomial hierarchy infinite?\n");
    printf("If yes, P != NP. If no, hierarchy collapses to some finite level.\n");
}

/* ================================================================
 * Stockmeyer Demo (1976)
 * ================================================================ */
void ph_demo_stockmeyer(void) {
    printf("\n===== Stockmeyer (1976): The Polynomial Hierarchy =====\n\n");
    printf("Larry Stockmeyer defined PH in his MIT PhD thesis (1976).\n");
    printf("Generalizing NP = exists x: R(x) to k alternations.\n\n");
    const char* names[] = {"P(Sigma_0)","NP(Sigma_1)","coNP(Pi_1)",
        "Sigma_2","Pi_2","Sigma_3","Pi_3","Sigma_4","Pi_4"};
    printf("  Level  Class        Complete Problem\n");
    printf("  -----  -----        -----------------\n");
    for (int i = 0; i <= 8; i++)
        printf("  %-5d  %-12s QSAT_%d\n", i, names[i], i);
    printf("  PH = union of all levels\n\n");

    printf("--- Demo: Sigma_2 SAT ---\n");
    printf("Formula: exists x0 forall x1: x0\n");
    PH_CNF* f1 = ph_cnf_build_single(0, 0, 0, 0, -1, -1);
    PH_QuantType t1[2] = {PH_QUANT_EXISTS, PH_QUANT_FORALL};
    int s1[2] = {1, 1};
    PH_QBF* q1 = ph_qbf_create(2, t1, s1, f1);
    int r1 = ph_sigma_k_sat(q1);
    printf("  Result: %s (expected SAT, pick x0=1)\n\n", r1?"SAT":"UNSAT");
    ph_qbf_free(q1);

    printf("--- Demo: Pi_2 SAT ---\n");
    printf("Formula: forall x0 exist x1: NOT x0 OR x1\n");
    PH_CNF* f2 = ph_cnf_build_single(0, 1, 1, 0, -1, -1);
    PH_QuantType t2[2] = {PH_QUANT_FORALL, PH_QUANT_EXISTS};
    int s2[2] = {1, 1};
    PH_QBF* q2 = ph_qbf_create(2, t2, s2, f2);
    int r2 = ph_pi_k_sat(q2);
    printf("  Result: %s (expected TRUE)\n\n", r2?"TRUE":"FALSE");
    ph_qbf_free(q2);

    printf("--- Hierarchy Structure ---\n");
    printf("        ...\n");
    printf("     Sigma_3  Pi_3\n");
    printf("     Sigma_2  Pi_2\n");
    printf("  NP=Sigma_1  Pi_1=coNP\n");
    printf("        P = Sigma_0 cap Pi_0\n\n");
    printf("Known: P subseteq NP subseteq Sigma_2 subseteq ... subseteq PSPACE\n");
    printf("Open: Are ALL inclusions strict?\n");
    printf("If P=NP, entire PH collapses to P.\n");
}

/* ================================================================
 * Toda Demo (1991)
 * ================================================================ */
void ph_demo_toda(void) {
    printf("\n===== Toda's Theorem (1991): PH subseteq P^{#P} =====\n\n");
    printf("Seinosuke Toda (Godel Prize 1998):\n");
    printf("  PH subseteq P^{#P}\n\n");
    printf("Counting is at least as powerful as quantifier alternation.\n\n");

    printf("--- #P Demonstration: #SAT ---\n");
    PH_CNF* f = ph_cnf_build_single(0, 0, 1, 0, 2, 0);
    f->n_vars = 3;
    long long cnt = ph_count_sat(f);
    printf("Formula: (x0|x1|x2) over 3 vars, #SAT=%lld (expected 7)\n\n", cnt);
    free(f);

    printf("--- Sigma_2 via #P ---\n");
    PH_CNF* f2 = ph_cnf_build_single(0, 0, 3, 0, -1, -1);
    f2->n_vars = 4; f2->n_clauses = 2;
    f2->clauses[1] = (PH_Clause){{1, (6<<1)|0, -1}};
    PH_QuantType t3[2] = {PH_QUANT_EXISTS, PH_QUANT_FORALL};
    int s3[2] = {2, 2};
    PH_QBF* q3 = ph_qbf_create(2, t3, s3, f2);
    int r3 = ph_sigma2_via_counting(q3);
    printf("  Sigma_2 via #P: %s\n", r3?"SAT":"UNSAT");
    ph_qbf_free(q3);

    printf("\n--- Proof Sketch ---\n");
    printf("1. Valiant-Vazirani: SAT -> Unique-SAT\n");
    printf("2. #P oracle counts solutions\n");
    printf("3. Binary search finds solution\n");
    printf("4. This simulates all levels of PH\n");
}

/* ================================================================
 * Karp-Lipton Demo (1982)
 * ================================================================ */
void ph_demo_karp_lipton(void) {
    printf("\n===== Karp-Lipton Theorem (1982): NP in P/poly => PH=Sigma_2 =====\n\n");
    printf("Theorem: If NP subseteq P/poly, then PH = Sigma_2.\n\n");
    printf("--- Proof Sketch ---\n");
    printf("Assume SAT has poly-size circuits C_n.\n");
    printf("For Pi_2 'forall y exist z: phi(y,z)':\n");
    printf("  1. 'exist z: phi' is NP, decided by circuit C\n");
    printf("  2. 'forall y C accepts' is coNP = Pi_1 subseteq Sigma_2\n");
    printf("  3. Thus Pi_2 subseteq Sigma_2, hierarchy collapses.\n\n");

    printf("--- Circuit Demo ---\n");
    PH_BoolCircuit* c = malloc(sizeof(PH_BoolCircuit));
    memset(c, 0, sizeof(PH_BoolCircuit));
    c->n_inputs = 2; c->n_gates = 4;
    c->gates[0] = (PH_Gate){PH_GATE_OR, 0, 1, 0};
    c->gates[1] = (PH_Gate){PH_GATE_NOT, 0, 0, 0};
    c->gates[2] = (PH_Gate){PH_GATE_OR, 3, 1, 0};
    c->gates[3] = (PH_Gate){PH_GATE_AND, 2, 4, 0};
    int in00[2]={0,0}, in01[2]={0,1}, in10[2]={1,0}, in11[2]={1,1};
    printf("Circuit: (x0|x1)&(!x0|x1) = x1\n");
    printf("  [0,0]->%d [0,1]->%d [1,0]->%d [1,1]->%d\n\n",
           ph_circuit_eval(c,in00), ph_circuit_eval(c,in01),
           ph_circuit_eval(c,in10), ph_circuit_eval(c,in11));
    free(c);
    printf("Corollary: proving NP not in P/poly => P != NP.\n");
}

/* ================================================================
 * Collapse Demo
 * ================================================================ */
void ph_demo_collapse(void) {
    printf("\n===== PH Collapse Theorems =====\n\n");
    printf("The Domino Effect:\n\n");
    printf("Theorem 1: If P=NP, then PH=P.\n");
    printf("  Proof: P=NP=Sigma_1, P=coP=Pi_1 => Sigma_1=Pi_1\n");
    printf("  => Sigma_2=NP^{NP}=NP=P, by induction PH=P.\n\n");
    printf("Theorem 2: If Sigma_k=Pi_k, then PH=Sigma_k.\n");
    printf("  Proof: Sigma_{k+1}=NP^{Sigma_k}=NP^{Pi_k}=Pi_k=Sigma_k.\n\n");
    printf("Theorem 3: If Sigma_k=Sigma_{k+1}, then PH=Sigma_k.\n\n");

    printf("--- Collapse Check (small instances) ---\n");
    ph_collapse_check(2);

    printf("\n--- Scenarios ---\n");
    printf("Scenario 1 (P=NP): PH=P, all NP-complete problems easy.\n");
    printf("Scenario 2 (NP=coNP): PH=NP, SAT hard but stays at level 1.\n");
    printf("Scenario 3 (Sigma_2=Pi_2): PH=Sigma_2.\n");
    printf("Scenario 4 (PH infinite): P!=NP, each level strict.\n");
}

/* ================================================================
 * Oracle Demo
 * ================================================================ */
void ph_demo_oracle(void) {
    printf("\n===== Oracle Definition of PH =====\n\n");
    printf("  Sigma_0 = P\n");
    printf("  Sigma_{k+1} = NP^{Sigma_k}\n");
    printf("  Pi_k = co-Sigma_k\n\n");
    printf("Sigma_1 = NP^P = NP (P oracle is useless)\n");
    printf("Sigma_2 = NP^{NP} = NP with SAT oracle\n");
    printf("Sigma_3 = NP^{Sigma_2}\n\n");
    printf("Sigma_2 = NP^{SAT}: guess x, ask SAT oracle\n");
    printf("  'is NOT phi(x,.) unsatisfiable?'\n");
    printf("  If SAT says NO, forall y holds => accept.\n\n");
    printf("Each oracle level = one quantifier alternation.\n");
}

/* ================================================================
 * Complete Problems Demo
 * ================================================================ */
void ph_demo_complete(void) {
    printf("\n===== PH-Complete Problems =====\n\n");
    printf("  Level  Class     Complete Problem(s)\n");
    printf("  0      P         Circuit Value, HornSAT\n");
    printf("  1      NP        SAT, CLIQUE, VC, HAM-PATH\n");
    printf("  1      coNP      UNSAT, TAUTOLOGY\n");
    printf("  2      Sigma_2   QSAT_2, MIN-DNF\n");
    printf("  2      Pi_2      co-QSAT_2\n");
    printf("  3      Sigma_3   QSAT_3\n");
    printf("  PH     -         PH-SAT\n");
    printf("  PSPACE -         TQBF\n\n");

    printf("--- Demo: Sigma_2 SAT ---\n");
    printf("exists x0,x1 forall x2: (x0|x1) & (!x1|!x2)\n");
    PH_CNF* f = malloc(sizeof(PH_CNF));
    memset(f, 0, sizeof(PH_CNF));
    f->n_vars = 3; f->n_clauses = 2;
    f->clauses[0] = (PH_Clause){{0, 2, -1}};
    f->clauses[1] = (PH_Clause){{(2<<1)|1, (4<<1)|1, -1}};
    PH_QuantType t[3] = {PH_QUANT_EXISTS, PH_QUANT_EXISTS, PH_QUANT_FORALL};
    int s[3] = {1, 1, 1};
    PH_QBF* q = ph_qbf_create(3, t, s, f);
    int r = ph_sigma_k_sat(q);
    printf("  Result: %s\n", r?"SAT":"UNSAT");
    ph_qbf_free(q);
}

/* ================================================================
 * QBF Demo
 * ================================================================ */
void ph_demo_qbf(void) {
    printf("\n===== QBF Solver Demo =====\n\n");
    printf("QBF is the unifying language for PH.\n\n");

    printf("--- Ex1: E x A y: (x|y)&(!x|!y) ---\n");
    PH_CNF* f1 = ph_cnf_build_single(0,0,1,0,-1,-1);
    f1->n_clauses = 2;
    f1->clauses[1] = (PH_Clause){{1,(2<<1)|1,-1}};
    PH_QuantType t1[2] = {PH_QUANT_EXISTS, PH_QUANT_FORALL};
    int s1[2] = {1,1};
    PH_QBF* q1 = ph_qbf_create(2, t1, s1, f1);
    printf("  E x A y: (x|y)&(!x|!y) => %s\n", ph_qbf_solve(q1)?"TRUE":"FALSE");
    ph_qbf_free(q1);

    printf("--- Ex2: A x E y: (x|y)&(!x|!y) ---\n");
    PH_CNF* f2 = ph_cnf_build_single(0,0,1,0,-1,-1);
    f2->n_clauses = 2;
    f2->clauses[1] = (PH_Clause){{1,(2<<1)|1,-1}};
    PH_QuantType t2[2] = {PH_QUANT_FORALL, PH_QUANT_EXISTS};
    int s2[2] = {1,1};
    PH_QBF* q2 = ph_qbf_create(2, t2, s2, f2);
    printf("  A x E y: (x|y)&(!x|!y) => %s\n", ph_qbf_solve(q2)?"TRUE":"FALSE");
    ph_qbf_free(q2);

    printf("\nQBF Complexity:\n");
    printf("  k=1 (E):   NP-complete (SAT)\n");
    printf("  k=2 (E-A): Sigma_2-complete\n");
    printf("  k=3:       Sigma_3-complete\n");
    printf("  unbounded: PSPACE-complete (TQBF)\n");
}

/* ================================================================
 * Oracle Full Demo
 * ================================================================ */
void ph_demo_oracle_full(void) {
    printf("\n===== Oracle Machine Simulation =====\n\n");
    printf("Nesting NP oracles gives PH levels:\n\n");
    printf("Level 0: P = plain SAT (DPLL)\n");
    printf("Level 1: NP = exists x: phi\n");
    printf("Level 2: NP^{SAT} = exists x, query SAT\n");
    printf("Level 3: NP^{NP^{SAT}} = double oracle nesting\n\n");

    printf("--- Sigma_2 via NP^{SAT} ---\n");
    PH_CNF* f = ph_cnf_build_single(0, 0, 0, 0, -1, -1);
    PH_QuantType t[2] = {PH_QUANT_EXISTS, PH_QUANT_FORALL};
    int s[2] = {1, 1};
    PH_QBF* q = ph_qbf_create(2, t, s, f);
    int r = ph_sigma2_oracle_solve(q);
    printf("  Oracle answer: %s\n", r ? "SAT" : "UNSAT");
    printf("  Oracle Hierarchy = Quantifier Hierarchy (Meyer-Stockmeyer)\n");
    ph_qbf_free(q);
}

/* ================================================================
 * Reductions Demo
 * ================================================================ */
void ph_demo_reductions(void) {
    printf("\n===== NP-Completeness Reductions =====\n\n");
    PH_CNF* f = malloc(sizeof(PH_CNF));
    memset(f, 0, sizeof(PH_CNF));
    f->n_vars = 3; f->n_clauses = 2;
    f->clauses[0] = (PH_Clause){{0, 2, 4}};
    f->clauses[1] = (PH_Clause){{1, (2<<1)|1, (4<<1)|1}};
    printf("Formula: (x0|x1|x2) & (!x0|!x1|!x2)\n\n");

    printf("--- CLIQUE ---\n");
    int cg[36]; int cn, ck;
    ph_sat_to_clique(f, cg, &cn, &ck);
    printf("  %d vertices, clique size %d\n", cn, ck);

    printf("--- VERTEX-COVER ---\n");
    int vg[36]; int vn, vk;
    ph_sat_to_vertexcover(f, vg, &vn, &vk);
    printf("  %d vertices, cover size %d\n", vn, vk);

    printf("--- 3-COLOR ---\n");
    int c3g[65536] = {0}; int c3n;
    ph_sat_to_3color(f, c3g, &c3n);
    printf("  %d vertices\n", c3n);

    printf("--- SUBSET-SUM ---\n");
    int ss[64]; int ssn, sst;
    ph_sat_to_subsetsum(f, ss, &ssn, &sst);
    printf("  %d numbers, target=%d\n", ssn, sst);

    printf("--- HAMILTONIAN-PATH ---\n");
    int hpg[40000] = {0}; int hpn;
    ph_sat_to_hampath(f, hpg, &hpn);
    printf("  %d vertices\n", hpn);

    printf("\nAll reductions are polynomial-time (Karp 1972).\n");
    printf("They demonstrate NP-completeness of all these problems.\n");
    free(f);
}
