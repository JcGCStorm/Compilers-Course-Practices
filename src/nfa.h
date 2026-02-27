#ifndef NFA_H
#define NFA_H

#include "regex.h"

typedef struct {
    // TODO
} nfa;

nfa regex_to_nfa(regex r);
int match_nfa(nfa n, const char *str, int len);

#endif
