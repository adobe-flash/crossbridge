/* APPLE LOCAL file Radar 4591706  */
/* Test generation of DWARF call site information for inlined subroutines 
   in C++  */

// { dg-do compile }
// { dg-options "-Os -gdwarf-2 -dA" }
// { dg-final { scan-assembler "\[#@;!/|\]+\[ \t\]+DW_AT_call_line" } }

#include <algorithm>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "time-sort.h"

int main()
{
	universal_time t0;
	unsigned N = 1000000;
	std::string* p = new std::string[N];
	universal_time t1;
	for (unsigned i = 0; i < N; ++i)
	{
		char buf[40];
		std::sprintf(buf, "12345678901234567890%u", i);
		p[i] = buf;
	}
	universal_time t2;
	std::random_shuffle(p, p+N);
	universal_time t3;
	std::sort(p, p+N);
	universal_time t4;
	delete [] p;
	universal_time t5;
	std::printf("\nN = %u\n", N);
	elapsed_time d = t1 - t0;
	std::printf("default construction time = %f\n", d.sec_ + d.nsec_/1.e9);
	d = t2 - t1;
	std::printf("assign time = %f\n", d.sec_ + d.nsec_/1.e9);
	d = t3 - t2;
	std::printf("random_shuffle time = %f\n", d.sec_ + d.nsec_/1.e9);
	d = t4 - t3;
	std::printf("sort time = %f\n", d.sec_ + d.nsec_/1.e9);
	d = t5 - t4;
	std::printf("destruct time = %f\n", d.sec_ + d.nsec_/1.e9);
	d = t5 - t0;
	std::printf("total time = %f\n", d.sec_ + d.nsec_/1.e9);
}
