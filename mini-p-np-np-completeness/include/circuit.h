/* circuit.h ? Boolean Circuit: non-uniform computation model.
 * P/poly, Karp-Lipton, circuit-SAT, AC0/NC1/TC0.
 * Reference: Arora-Barak SS6, Vollmer SS1
 */
#ifndef CIRCUIT_H
#define CIRCUIT_H
#include "types.h"

typedef enum { GATE_INPUT, GATE_AND, GATE_OR, GATE_NOT } GateType;

typedef struct {
    int id; GateType type; int var;
    int in[2]; int nin; int value;
} Gate;

typedef struct {
    Gate* gates; int n_gates, n_inputs, n_input_gates, output_id;
} Circuit;

Circuit* circuit_new(int n_inputs, int max_gates);
int circuit_add_input(Circuit* c, int var);
int circuit_add_and(Circuit* c, int in0, int in1);
int circuit_add_or(Circuit* c, int in0, int in1);
int circuit_add_not(Circuit* c, int in0);
void circuit_set_output(Circuit* c, int gate_id);
int circuit_eval(const Circuit* c, const int* input);
int circuit_size(const Circuit* c);
int circuit_depth(const Circuit* c);
int circuit_sat(const Circuit* c);
CNF* circuit_to_cnf(const Circuit* c);
void circuit_free(Circuit* c);
void circuit_print(const Circuit* c);
Circuit* circuit_majority(int n);
Circuit* circuit_parity(int n);
Circuit* circuit_from_cnf(const CNF* cnf);

#endif
