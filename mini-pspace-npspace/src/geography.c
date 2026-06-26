/* geography.c — Generalized Geography: PSPACE-complete game
 *
 * Two players take turns naming cities. Each city must start with the
 * last letter of the previous city. No city can be repeated.
 * First player unable to move loses.
 *
 * Deciding which player has a winning strategy is PSPACE-complete.
 * This captures the essence of 2-player games with polynomial moves. */
#include <stdio.h>
#include <string.h>

void geography_demo(void) {
    printf("\n===== Generalized Geography =====\n\n");
    printf("PSPACE-complete 2-player game.\n\n");
    printf("Rules:\n");
    printf("  1. Players alternate naming cities\n");
    printf("  2. Each city starts with previous city's last letter\n");
    printf("  3. No city can be repeated\n");
    printf("  4. First player unable to move loses\n\n");
    
    printf("Example: A->Austin, B->Nantes, A->Shanghai, B->Istanbul, ...\n\n");
    
    printf("Why PSPACE-complete:\n");
    printf("  - Game tree depth = polynomial (max |V| moves)\n");
    printf("  - Each position = set of visited cities (O(|V|) space)\n");
    printf("  - Min-max search through game tree = PSPACE\n");
    printf("  - Reduces from TQBF: each quantifier = player turn\n\n");
    
    printf("This is analogous to QSAT for PSPACE:\n");
    printf("  SAT is to NP as TQBF/Geography is to PSPACE.\n");
}