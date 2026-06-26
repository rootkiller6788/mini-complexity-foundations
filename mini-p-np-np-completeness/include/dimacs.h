/* dimacs.h — DIMACS CNF format parser/writer */
#ifndef DIMACS_H
#define DIMACS_H
#include "types.h"

/* Parse DIMACS file. Returns CNF* or NULL on error. */
CNF* dimacs_parse(const char* filename);

/* Write CNF to DIMACS file */
int  dimacs_write(const CNF* cnf, const char* filename);

/* Parse from string (for testing) */
CNF* dimacs_parse_string(const char* str);

/* Helper: comment line starting with 'c' */
int  dimacs_is_comment(const char* line);

#endif
