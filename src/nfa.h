#ifndef NFA_H
#define NFA_H

#include "regex.h"

// Whith this we define the transition and state structures, 
// the transition structure has a symbol, a flag to indicate if its an epsilon transition, 
// a pointer to the state it goes to and a pointer to the next transition in the list of transitions for a state.
typedef struct transition {
    char symbol;   
    int is_epsilon;
    struct state *to;
    struct transition *next;  
} transition;

// The state structure has an id and a pointer to the list of transitions. 
typedef struct state {
    int id;
    transition *transitions;
} state;

// The NFA structure has a pointer to the start state and a pointer to the accept state.
typedef struct {
    state *start;
    state *accept;
} nfa;

// The fragment structure is used during the construction of the NFA, it has a pointer to the start 
// state and a pointer to the accept state of the fragment. 
typedef struct {
    state *start;
    state *accept;
} fragment;

nfa regex_to_nfa(regex r);
int match_nfa(nfa n, const char *str, int len);

#endif
