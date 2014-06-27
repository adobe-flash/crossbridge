// -*- mode: c++ -*-
// $Id: methcall.cpp 36673 2007-05-03 16:55:46Z laurov $
// http://www.bagley.org/~doug/shootout/

// with some help from Bill Lear

#include <stdlib.h>
#include <iostream>

using namespace std;

class Toggle {
public:
    Toggle(bool start_state) : state(start_state) { }
    virtual ~Toggle() {  }
    bool value() {
	return(state);
    }
    virtual Toggle& activate() {
	state = !state;
	return(*this);
    }
    bool state;
};

class NthToggle : public Toggle {
public:
    NthToggle(bool start_state, int max_counter) :
	Toggle(start_state), count_max(max_counter), counter(0) {
    }
    Toggle& activate() {
	if (++this->counter >= this->count_max) {
	    state = !state;
	    counter = 0;
	}
	return(*this);
    }
private:
    int count_max;
    int counter;
};


int
main(int argc, char *argv[]) {
#ifdef SMALL_PROBLEM_SIZE
#define LENGTH 100000000
#else
#define LENGTH 1000000000
#endif
    int n = ((argc == 2) ? atoi(argv[1]) : LENGTH);

    bool val = true;
    Toggle *toggle = new Toggle(val);
    for (int i=0; i<n; i++) {
	val = toggle->activate().value();
    }
    cout << ((val) ? "true" : "false") << endl;
    delete toggle;

    val = true;
    NthToggle *ntoggle = new NthToggle(val, 3);
    for (int i=0; i<n; i++) {
	val = ntoggle->activate().value();
    }
    cout << ((val) ? "true" : "false") << endl;
    delete ntoggle;

    return 0;
}
