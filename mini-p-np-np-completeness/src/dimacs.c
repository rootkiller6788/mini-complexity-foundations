/* dimacs.c — DIMACS CNF parser */
#include "dimacs.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* skip whitespace */
static const char* skip(const char* s) {
    while (*s && isspace(*s)) s++;
    return s;
}

CNF* dimacs_parse_string(const char* str) {
    int n_vars = 0, n_clauses = 0;
    CNF* cnf = NULL;
    int clause_count = 0;

    const char* p = str;
    char line[4096];

    while (*p) {
        /* read one line */
        const char* eol = strchr(p, '\n');
        size_t len = eol ? (size_t)(eol - p) : strlen(p);
        if (len >= sizeof(line)) len = sizeof(line) - 1;
        memcpy(line, p, len); line[len] = 0;
        p = eol ? eol + 1 : p + strlen(p);

        const char* s = skip(line);
        if (!*s || *s == 'c') continue; /* comment */

        if (*s == 'p') {
            char fmt[16]; int nv, nc;
            if (sscanf(s, "p %s %d %d", fmt, &nv, &nc) == 3) {
                n_vars = nv; n_clauses = nc;
                cnf = cnf_new(n_vars, n_clauses);
            }
            continue;
        }

        /* clause line */
        if (cnf && clause_count < n_clauses) {
            int lits[256], n = 0;
            const char* t = s;
            while (*t) {
                int val;
                if (sscanf(t, "%d", &val) == 1) {
                    if (val == 0) break;
                    /* DIMACS: 1..n for positive, -1..-n for negative */
                    int var = abs(val) - 1, sign = (val < 0) ? 1 : 0;
                    lits[n++] = (var << 1) | sign;
                    while (*t && !isspace(*t)) t++;
                    t = skip(t);
                } else break;
            }
            if (n > 0) { cnf_add(cnf, lits, n); clause_count++; }
        }
    }
    return cnf;
}

CNF* dimacs_parse(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = (char*)malloc((size_t)sz + 1);
    if (buf) {
        fread(buf, 1, (size_t)sz, f);
        buf[sz] = 0;
    }
    fclose(f);
    if (!buf) return NULL;
    CNF* cnf = dimacs_parse_string(buf);
    free(buf);
    return cnf;
}

int dimacs_write(const CNF* cnf, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) return -1;
    cnf_print(cnf);
    /* redirect print to file */
    /* simplified: just write directly */
    fprintf(f, "p cnf %d %d\n", cnf->n_vars, cnf->n_clauses);
    for (int i = 0; i < cnf->n_clauses; i++) {
        for (int j = 0; j < cnf->clauses[i].n; j++) {
            int lit = cnf->clauses[i].data[j];
            int dimacs_lit = (lit & 1) ? -(int)(lit >> 1) - 1 : (int)(lit >> 1) + 1;
            fprintf(f, "%d ", dimacs_lit);
        }
        fprintf(f, "0\n");
    }
    fclose(f);
    return 0;
}
