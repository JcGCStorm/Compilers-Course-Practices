#include <stdlib.h>
#include <string.h>
#include "nfa.h"

// We define a fragment, which is just a pair of states, a start state and an 
// accept state. We use a regular stack but instead of storing characters, we 
// store fragments of the NFA
typedef struct {
    fragment *data;
    int top;
    int capacity;
} fragment_stack;

// With these functions we can manipulate the stack of fragments, we can push
// and pop fragments from the stack, we can also resize the stack dinnamically.
void push_fragment(fragment_stack *s, fragment f) {
    if (s->top == s->capacity - 1) {
        s->capacity *= 2;
        s->data = realloc(s->data, s->capacity * sizeof(fragment));
    }
    s->data[++s->top] = f;
}

fragment pop_fragment(fragment_stack *s) {
    return s->data[s->top--];
}

// This function creates a new state with a unique id and no transitions.
state *create_state() {
    static int id_counter = 0;

    state *s = malloc(sizeof(state));
    s->id = id_counter++;
    s->transitions = NULL;

    return s;
}

// These functions are used to add transitions between states, we can add a normal
// transition with a symbol, or we can add an epsilon transition, which is a
// transition that can be taken without consuming any input symbol.
void add_transition(state *from, state *to, char symbol) {
    transition *t = malloc(sizeof(transition));
    t->symbol = symbol;
    t->is_epsilon = 0;
    t->to = to;
    t->next = from->transitions;

    from->transitions = t;
}

void add_epsilon_transition(state *from, state *to) {
    transition *t = malloc(sizeof(transition));
    t->symbol = '\0';
    t->is_epsilon = 1;
    t->to = to;
    t->next = from->transitions;

    from->transitions = t;
}

// These functions are used to create fragments for the basic regex constructs, 
// we can create a fragment for a single character, we can concatenate two fragments,
// we can alternate two fragments, we can apply the Kleene star to a fragment, 
// we can apply the plus operator to a fragment, and we can apply the question 
// mark operator to a fragment.
fragment create_char_fragment(char c) {
    state *start = create_state();
    state *accept = create_state();
    add_transition(start, accept, c);
    return (fragment){start, accept};
}

fragment concatenate_fragments(fragment f1, fragment f2) {
    add_epsilon_transition(f1.accept, f2.start);
    return (fragment){f1.start, f2.accept};
}

fragment alternate_fragments(fragment f1, fragment f2) {
    state *start = create_state();
    state *accept = create_state();

    add_epsilon_transition(start, f1.start);
    add_epsilon_transition(start, f2.start);
    add_epsilon_transition(f1.accept, accept);
    add_epsilon_transition(f2.accept, accept);

    return (fragment){start, accept};
}

fragment kleene_fragment(fragment f) {
    state *start = create_state();
    state *accept = create_state();

    add_epsilon_transition(start, f.start);
    add_epsilon_transition(start, accept);
    add_epsilon_transition(f.accept, f.start);
    add_epsilon_transition(f.accept, accept);

    return (fragment){start, accept};
}

fragment plus_fragment(fragment f) {
    state *start = create_state();
    state *accept = create_state();

    add_epsilon_transition(start, f.start);
    add_epsilon_transition(f.accept, f.start);
    add_epsilon_transition(f.accept, accept);

    return (fragment){start, accept};
}

fragment question_fragment(fragment f) {
    state *start = create_state();
    state *accept = create_state();

    add_epsilon_transition(start, f.start);
    add_epsilon_transition(start, accept);
    add_epsilon_transition(f.accept, accept);

    return (fragment){start, accept};
}


// This function takes a regex in postfix notation and constructs the corresponding 
// NFA using Thompson's construction algorithm.
// We iterate through the tokens of the regex, and for each token we perform the
// corresponding operation on the stack of fragments. For example, if we encounter 
// a character token, we create a new fragment for that character and push it onto 
// the stack. If we encounter a concatenation operator, we pop the top two fragments
// from the stack, concatenate them, and push the resulting fragment back onto the stack.
// If we encounter an alternation operator, we pop the top two fragments from the stack,
// alternate them, and push the resulting fragment back onto the stack. If we encounter a
// Kleene star operator, we pop the top fragment from the stack, apply the Kleene star to it,
// and push the resulting fragment back onto the stack. If we encounter a plus operator, we
// pop the top fragment from the stack, apply the plus operator to it, and push the resulting
// fragment back onto the stack. If we encounter a question mark operator, we pop the top
// fragment from the stack, apply the question mark operator to it, and push the resulting
// fragment back onto the stack. At the end of the iteration, we should have a single
// fragment on the stack, which represents the NFA for the entire regex.
// At the end, we should have a single fragment on the stack, which represents the 
// NFA for the entire regex.
nfa regex_to_nfa(regex r)
{
    if (r.size == 0) {
        state *start = create_state();
        state *accept = create_state();
        add_epsilon_transition(start, accept);
        return (nfa){start, accept};
    }

    fragment_stack stack;
    stack.data = malloc(r.size * sizeof(fragment));
    stack.top = -1;
    stack.capacity = r.size;

    for (int i = 0; i < r.size; i++) {
        token t = r.items[i];
        fragment f, f1, f2;

        switch (t.type) {
            case TOKEN_CHAR:
                f = create_char_fragment(t.value);
                push_fragment(&stack, f);
                break;
            case TOKEN_DOT:
                f2 = pop_fragment(&stack);
                f1 = pop_fragment(&stack);
                push_fragment(&stack, concatenate_fragments(f1, f2));
                break;
            case TOKEN_PIPE:
                f2 = pop_fragment(&stack);
                f1 = pop_fragment(&stack);
                push_fragment(&stack, alternate_fragments(f1, f2));
                break;
            case TOKEN_STAR:
                f = pop_fragment(&stack);
                push_fragment(&stack, kleene_fragment(f));
                break;
            case TOKEN_PLUS:
                f = pop_fragment(&stack);
                push_fragment(&stack, plus_fragment(f));
                break;
            case TOKEN_QUESTION:
                f = pop_fragment(&stack);
                push_fragment(&stack, question_fragment(f));
                break;
            default:
                break;
        }
    }
    fragment result = pop_fragment(&stack);
    free(stack.data);
    return (nfa){result.start, result.accept};
}

// We define a state set, which is just a dynamic array of states, we use this to 
// keep track of the current set of states in the NFA simulation.
typedef struct {
    state **states;
    int size;
    int capacity;
} state_set;

// With these functions we can manipulate the state set, we can create a new state 
// set, we can free a state set, we can check if a state is in the set, and we can 
// add a state to the set.
state_set *create_state_set(int capacity) {
    state_set *set = malloc(sizeof(state_set));
    set->states = malloc(capacity * sizeof(state*));
    set->size = 0;
    set->capacity = capacity;
    return set;
}

void free_state_set(state_set *set) {
    free(set->states);
    free(set);
}

int contains_state(state_set *set, state *s) {
    for (int i = 0; i < set->size; i++) {
        if (set->states[i] == s) return 1;
    }
    return 0;
}

void add_state(state_set *set, state *s) {
    if (!contains_state(set, s)) {
        if (set->size == set->capacity) {
            set->capacity *= 2;
            set->states = realloc(set->states, set->capacity * sizeof(state*));
        }
        set->states[set->size++] = s;
    }
}

// This function computes the epsilon closure of a state, which is the set of states
//  that can be reached from the given state by following epsilon transitions. 
// We use this function in the NFA simulation to keep track of all the states 
// that we can be in at any given time.
void epsilon_closure(state_set *set, state *s) {
    add_state(set, s);
    for (transition *t = s->transitions; t != NULL; t = t->next) {
        if (t->is_epsilon && !contains_state(set, t->to)) {
            epsilon_closure(set, t->to);
        }
    }
}

// This function takes a set of states and an input symbol, and returns the set of 
// states that can be reached from the current set of states by following 
// transitions 
state_set *move(state_set *current, char symbol) {
    state_set *next = create_state_set(current->capacity);
    for (int i = 0; i < current->size; i++) {
        state *s = current->states[i];
        for (transition *t = s->transitions; t != NULL; t = t->next) {
            if (!t->is_epsilon && t->symbol == symbol) {
                epsilon_closure(next, t->to);
            }
        }
    }

    return next;
}


// This function simulates the NFA on a given input string, it returns 1 if the
// NFA accepts the string, and 0 otherwise. We start by computing the epsilon closure
// of the start state, which gives us the initial set of states. Then we iterate through
// the input string, and for each symbol we compute the next set of states using the
// move function. If at any point the set of states becomes empty, we can stop and
// return 0, because the NFA cannot accept the string. At the end of the iteration,
// we check if the accept state is in the current set of states, if it is,
// we return 1, otherwise we return 0.
int match_nfa(nfa n, const char *str, int len)
{
    state_set *current = create_state_set(64);
    epsilon_closure(current, n.start);
    for (int i = 0; i < len; i++) {
        state_set *next = move(current, str[i]);
        free_state_set(current);
        current = next;

        if (current->size == 0) {
            break;
        }
    }
    int accepted = 0;
    for (int i = 0; i < current->size; i++) {
        if (current->states[i] == n.accept) {
            accepted = 1;
            break;
        }
    }
    free_state_set(current);
    return accepted;
}
