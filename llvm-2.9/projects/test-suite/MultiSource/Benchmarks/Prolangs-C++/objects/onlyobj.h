// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  Written by Ashok Sreenivasan, TRDDC, Pune, India.  1993.  May be
//  distributed freely, provided this comment is displayed at the top.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#ifndef __OBJ__

class Object
{
	public :

// Constructor - Dummy, useful for breakpoints !

		Object () {}

// Destructor - also dummy but also virtual

		virtual ~Object () {}

//	(In)Equality operators

		virtual int operator == (Object &) = 0;
		int operator != (Object &o) { return !(*this == o); }

// Type enquiry function

		virtual char *Type () { return "Object"; }
};

typedef Object *Objp;

#define __OBJ__
#endif
