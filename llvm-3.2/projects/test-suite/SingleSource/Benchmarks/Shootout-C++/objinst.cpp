// -*- mode: c++ -*-
// $Id: objinst.cpp 37514 2007-06-08 06:33:15Z evancheng $
// http://www.bagley.org/~doug/shootout/

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
#define LENGTH 1000000
#else
#define LENGTH 70000000
#endif
    int n = ((argc == 2) ? atoi(argv[1]) : LENGTH);

    Toggle *toggle1 = new Toggle(true);
    for (int i=0; i<5; i++) {
	cout << ((toggle1->activate().value()) ? "true" : "false") << endl;
    }
    delete toggle1;
    for (int i=0; i<n; i++) {
	Toggle *toggle = new Toggle(true);
	delete toggle;
    }
    
    cout << endl;
    
    NthToggle *ntoggle1 = new NthToggle(true, 3);
    for (int i=0; i<8; i++) {
	cout << ((ntoggle1->activate().value()) ? "true" : "false") << endl;
    }
    delete ntoggle1;
    for (int i=0; i<n; i++) {
	NthToggle *ntoggle = new NthToggle(true, 3);
	delete ntoggle;
    }
    return 0;
}
