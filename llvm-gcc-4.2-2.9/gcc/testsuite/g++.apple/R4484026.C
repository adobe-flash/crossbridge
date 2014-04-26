/* { dg-options "-fpreprocessed" } */
/* { dg-compile } */
# 1 "blah.h"
class A
{
	public:
	enum my_enum 
	{
		one,
		two,
# 1 "blah.h" 1
# 1 "blah.h" 2
		three
	};
};

