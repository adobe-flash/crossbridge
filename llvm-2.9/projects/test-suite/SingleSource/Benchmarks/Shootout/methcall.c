/* -*- mode: c -*-
 * $Id: methcall.c 36673 2007-05-03 16:55:46Z laurov $
 * http://www.bagley.org/~doug/shootout/
 */

#include <stdio.h>
#include <stdlib.h>


enum {false, true};

#define TOGGLE \
    char state; \
    char (*value)(struct Toggle *); \
    struct Toggle *(*activate)(struct Toggle *)

#define DESTROY  free

typedef struct Toggle {
    TOGGLE;
} Toggle;

char toggle_value(Toggle *this) {
    return(this->state);
}
Toggle *toggle_activate(Toggle *this) {
    this->state = !this->state;
    return(this);
}
Toggle *init_Toggle(Toggle *this, char start_state) {
    this->state = start_state;
    this->value = toggle_value;
    this->activate = toggle_activate;
    return(this);
}
Toggle *new_Toggle(char start_state) {
    Toggle *this = (Toggle *)malloc(sizeof(Toggle));
    return(init_Toggle(this, start_state));
}


typedef struct NthToggle {
    TOGGLE;
    int count_max;
    int counter;
} NthToggle;

NthToggle *nth_toggle_activate(NthToggle *this) {
    if (++this->counter >= this->count_max) {
	this->state = !this->state;
	this->counter = 0;
    }
    return(this);
}
NthToggle *init_NthToggle(NthToggle *this, int max_count) {
    this->count_max = max_count;
    this->counter = 0;
    this->activate = (Toggle *(*)(Toggle *))nth_toggle_activate;
    return(this);
}
NthToggle *new_NthToggle(char start_state, int max_count) {
    NthToggle *this = (NthToggle *)malloc(sizeof(NthToggle));
    this = (NthToggle *)init_Toggle((Toggle *)this, start_state);
    return(init_NthToggle(this, max_count));
}


int main(int argc, char *argv[]) {
#ifdef SMALL_PROBLEM_SIZE
#define LENGTH 50000000
#else
#define LENGTH 500000000
#endif
    int i, n = ((argc == 2) ? atoi(argv[1]) : LENGTH);
    Toggle *tog;
    NthToggle *ntog;
    char val = true;

    tog = new_Toggle(true);
    for (i=0; i<n; i++) {
	val = tog->activate(tog)->value(tog);
    }
    puts(val ? "true\n" : "false\n");
    DESTROY(tog);
    
    val = true;
    ntog = new_NthToggle(val, 3);
    for (i=0; i<n; i++) {
	val = ntog->activate(ntog)->value(ntog);
    }
    puts(val ? "true\n" : "false\n");
    DESTROY(ntog);
    return 0;
}
