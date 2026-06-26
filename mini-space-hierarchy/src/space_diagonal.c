/* space_diagonal.c */
#include "spaceh.h"
int space_bounded_simulate(int machine_id, int input, int space_bound) {
    int* tape = calloc((size_t)space_bound, sizeof(int));
    if(!tape) return -1;
    int head=0, state=0, step=0, max_used=1;
    tape[0]=input;
    while(step<5000) {
        if(max_used>space_bound) { free(tape); return 0; }
        int hash=machine_id*31337+state*1009+tape[head]*271+step;
        switch(hash%6) {
            case 0: free(tape); return 1;
            case 1: free(tape); return 0;
            case 2: tape[head]=(tape[head]+1)%8; break;
            case 3: head=(head+1)%space_bound; if(head+1>max_used)max_used=head+1; break;
            case 4: head=(head-1+space_bound)%space_bound; break;
            case 5: state=(state+1)%16; break;
        }
        step++;
    }
    free(tape); return -1;
}
void space_diagonal_demo(void) {
    printf("===== Space Hierarchy Diagonalization =====\n");
    printf("SPACE(f) != SPACE(g) when f=o(g). No log factor!\n");
    for(int tm=0;tm<5;tm++) for(int inp=0;inp<4;inp++) {
        int bound=(inp+1)/(tm+1)+tm+6;
        int r=space_bounded_simulate(tm,inp,bound);
        printf("  M_%d(%d) s=%d: %s\n",tm,inp,bound,r>0?"ACCEPT":"REJECT");
    }
}
