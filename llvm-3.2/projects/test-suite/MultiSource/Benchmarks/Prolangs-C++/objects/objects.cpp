// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  Written by Ashok Sreenivasan, TRDDC, Pune, India.  1993.  May be
//  distributed freely, provided this comment is displayed at the top.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "onlyobj.h"
static int strlen(char *str1) {return 0;}
static int strcmp(char *str1, char *str2) {return 0;}
static char *strcpy(char *str1, char *str2) {return "";}
static char *strncpy(char *str1, char *str2, int i) {return "";}
static char *strcat(char *str1, char *str2) {return "";}

#include "artest.h"

#define abs(x)		((x) > 0 ? (x) : (-x))

// The search function for a table searches for the element linearly and
// returns the index of the element in the list.  If not found, a -1 is
// returned.  The comparison of objects is done through the '==' operator.

int Table::Search (Object &obj) 
{
	for (int i = 0; i < nelem; i++)
		if (obj == * (Get(i)))
			return i;
	return -1;
}

// The table == operator is used to check for equality of tables themselves!
// This operator checks first if the types of the two tables are the same,
// then if the no. of elements are the same, and finally if each and every
// element is the same.  Only then are the tables deemed equal.  Note that
// if the tables are large, this could be an expensive operation.

int Table::operator == (Object &tblobj) 
{
	char *a;
	if (strcmp (a=tblobj.Type (), Type ()))						// Type check
		return 0;
	Table *tblptr = (Table *) &tblobj;
	if (tblptr->Nelem () == nelem)								// Count check
	{
	Object *b;
		for (int i = 0; i < nelem; i++)
			if (*(b=tblptr->Get (i)) != *(Get(i)))				// Elementwise check
				return 0;
	}
	else
		return 0;
	return 1;
}

// The array constructor allocates the reqd space and initialises the elems
// explicitly to NULL.

Array::Array (int sz, enum TblType t) : Table (t)
{
	array = new Objp [sz];
	size = sz;
	for (int i = 0; i < sz; i++)				// Explicitly NULLify
		array [i] = 0;
}

// The array destructor - deletes the elements from the rightmost to leftmost
// first.  Once the elements are rid, the array pointer itself is deleted.

Array::~Array ()
{
	if (type == VOLAT)
	{
		for (int i = nelem - 1; i >= 0; i--)
			Remove (i);
	}

	if (array)
	{
		delete array;
		array = 0;
	}
	Init ();
}

// The array append function increments nelem and sets curind and adds the
// passed element at the end.  Appending to an empty array is an error.

int Array::Append (Object *o)
{
	if (nelem == size)
		return 0;
	array [curind = nelem] = o;
	nelem ++;
	return 1;
}

// The Array insert function inserts the object at the given position and
// moves the objects to the right by one position as required.  Cannot insert
// far to the right of rightmost element.  Returns a boolean success / fail
// value.  Insertion also fails when the array is full.

int Array::Insert (Object *obj, int pos)
{
	if (pos > nelem || pos < 0 || nelem >= size)
		return 0;
	for (int i = nelem; i > pos; i--)
		array [i] = array [i - 1];					// Move objects right.
	array [pos] = obj;
	nelem ++;
	curind = pos;
	return 1;
}

// The Array assign function overwrites the object at the given position with
// the passed object if the passed position is valid.

int Array::Assign (Object *obj, int pos)
{
	if (pos > nelem || pos < 0 || nelem >= size)
		return 0;
	if (pos == nelem)
		nelem ++;									// Adding new element
	array [curind = pos] = obj;
	return 1;
}

// The array get function just performs a simple array access on the array
// after validating bounds.

Object *Array::Get (int idx) 
{
	if (idx >= nelem || idx < 0)
		return 0;
	curind = idx;
	return array [idx];
}

// The Fetch function gets an object from a list and returns it to the caller
//	without deleting it.  It also takes care of shifting elements 
// appropriately etc.

Object *Array::Fetch (int idx)
{
	if (idx >= nelem || idx < 0)
		return 0;
	Object *ret = Get (idx);
	for (int i = idx; i < nelem - 1; i++)
		array [i] = array [i + 1];
	array [nelem - 1] = 0;
	if (idx == nelem - 1)							// Right most element
		curind = idx - 1;
	else
		curind = idx;
	nelem --;
	return ret;
}

// The assign function is the only way to add / overwrite elements in a 
// sparse array.  It just overwrites the element in the position with the new
// object, while deleting the old one if one existed.

int SpArray::Assign (Object *o, int pos)
{
	if (pos >= size || pos < 0)
		return 0;
	if (array [pos] == 0)
		nelem ++;									// increase element count
	array [curind = pos] = o;
	return 1;
}

// The Get function gets the required array element and returns it.

Object *SpArray::Get (int idx) 
{
	if (idx >= size || idx < 0)
		return 0;
	curind = idx;
	return array [idx];
}

// The Fetch function just gets an array element and returns it, and forgets
// it from the sparse array.

Object *SpArray::Fetch (int pos)
{
	Object *ret = Get (pos);
	if (ret)												// Location was occupied
	{
		array [pos] = 0;
		if (pos == nelem - 1)
			curind = pos - 1;
		else
			curind = pos;
		nelem --;
	}
	return ret;
}

// Search all the elements in the array - as nelem is no limiting factor !

int SpArray::Search (Object &o) 
{
	for (int i = 0; i < size; i++)
	{
		Object *p = array [i];
		if (p != 0)								// Location is occupied
			if (*p == o)
				return i;
	}
	return -1;
}

// The sparse array destructor - deletes the elements from the rightmost to
// leftmost first.  Once the elements are rid, the array pointer itself is
// deleted.

SpArray::~SpArray ()
{
	if (type == VOLAT)
	{
		for (int i = size - 1; i >= 0; i--)
			if (array [i] != 0)
				Remove (i);
	}

	if (array)
	{
		delete array;
		array = 0;
	}
	Init ();
}

class A : public Object
{
        public :
                int operator == (Object &o)
                        {	char *a;
				if (strcmp (a=o.Type (),Type ())) return 0;
                                return i == ((A&)o).i; }
                char *Type () { return "A"; }
                A (int j = 0) : i(j) {}
                operator int () { return i; }
                ~A () {}
        protected :
                int i;
};

class B : public A
{
        public :
                int operator == (Object &o)
			{	char *a;
                                if (strcmp (a=o.Type (),Type ())) return 0;
                                return j == ((B&)o).j; }
                char *Type () { return "B"; }
                B (int j=0) {}
                operator int () { return j; }
                ~B () {}
        protected :
                int j;
};

 
int main ()
{
        A *a1 = new A(1);
        A *a2 = new A(2);
	A *a3;
        Array *ar = new Array(3,VOLAT);
	SpArray *sar = new SpArray(3,VOLAT);
	Array *a;
	if (0)
		a = ar;
	else
		a = sar;
	a->Assign(a1);
	a->Assign(a2, 2);
	a3 = (A *)a->Fetch(2);
	a3->Type();
}
