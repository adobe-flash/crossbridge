// light.cc

#include "broken_light.h"
#include "stream.h"

void light::tick()
{
    time_remaining_in_current_state--;
    if (time_remaining_in_current_state <= 0)
    {
        current_state = this->next_state();

	//cout << "current state = "<< current_state << '\n';
	time_remaining_in_current_state = time_in_state[current_state];
    }
}



void light::init(int t1, int t2, int t3, int t4) 
{
    __ = LIGHT_ID;
    current_state = 0;
    time_in_state[0] = t1;
    time_in_state[1] = t2;
    time_in_state[2] = t3;
    time_in_state[3] = t4;
    time_remaining_in_current_state = time_in_state[current_state];
}


ostream& operator<< (ostream & o, light l)
{
    o << l.current_state 
      << " " << l.redNS() << l.yellowNS() << l.greenNS() 
      << " " << l.redEW() << l.yellowEW() << l.greenEW();
    return(o);
}

#ifdef test_light
#include "stream.h"

void main()
{
    light l(3,1,4,1);

    for(int i = 0; i < 20; i++)
    {
	cout << l << "\n";
	l.tick();
    }
}
#endif
