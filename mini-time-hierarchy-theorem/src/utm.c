#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tht.h"

int utm_simulate(int machine_id, int input, int time_bound) {
    int n_states = (machine_id % 8) + 2;
    int state = 0;
    int tape[1024] = {0};
    int head = 512;
    int tmp = input, pos = 0;
    if (tmp == 0) tape[head] = 1;
    while (tmp > 0) { tape[head + pos] = (tmp % 2) + 1; tmp /= 2; pos++; }
    for (int step = 0; step < time_bound; step++) {
        if (state == n_states - 1) return 1;
        if (state == n_states - 2) return 0;
        int sym = tape[head];
        int hash = machine_id * 7919 + state * 6271 + sym * 4093 + step;
        int new_sym = hash % 3;
        int new_state = (hash / 3) % n_states;
        int move = ((hash / 3 / n_states) % 3) - 1;
        tape[head] = new_sym;
        state = new_state;
        head += move;
        if (head < 0 || head >= 1024) break;
    }
    return -1;
}

void utm_demo(void) {
    printf("\n===== Universal Turing Machine =====\n\n");
    printf("A UTM simulates any TM with O(T log T) overhead.\n");
    printf("Essential for the time hierarchy proof.\n\n");
    printf("--- UTM Simulation Demo ---\n");
    for (int tm_id = 0; tm_id < 6; tm_id++) {
        for (int inp = 0; inp < 4; inp++) {
            int r = utm_simulate(tm_id, inp, 50);
            printf("  UTM(TM_%d, %d) = %s\n", tm_id, inp,
                   r==1?"ACCEPT":(r==0?"REJECT":"TIMEOUT"));
        }
    }
    printf("\nThe UTM enables the diagonal language L_D to be in TIME(g).\n");
}
