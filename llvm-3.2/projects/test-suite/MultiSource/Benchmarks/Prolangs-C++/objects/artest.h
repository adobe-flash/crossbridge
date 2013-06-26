// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  Written by Ashok Sreenivasan, TRDDC, Pune, India.  1993.  May be
//  distributed freely, provided this comment is displayed at the top.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#ifndef __TBL__



#ifndef TRUE
#define TRUE (1)
#define FALSE (0)
#endif

enum TblType
{
	PERS,											// For "persistent" tables
	VOLAT,										// For "volatile" tables
};

class Table : public Object
{
	public:

// Constructor

		Table (enum TblType t = PERS) : type (t) { Init (); }

// Insertion functions

		virtual int Insert (Object *, int pos=0) = 0;
		virtual int Append (Object *p) { return Insert (p, Nelem ()); }
		Table &operator << (Object *p)	{ Append (p); return *this; }

// Overwrite (assign) function

		virtual int Assign (Object *o, int pos = 0)
			{ Fetch (pos); return Insert (o, pos); }

// Fetch functions

		virtual Object *Get (int pos) = 0;
		Object *Cur () { return Get (CurInd ()); }
		Object *First () { return Get (0); }
		Object *Next () { return Get (CurInd()+1); }
		Object *Prev () { return Get (CurInd()-1); }
		Object *Last () { return Get (Nelem()-1); }
		Object *operator [] (int pos) { return Get (pos); }

// Removal functions
// Remove functions delete the object, while Fetch functions get the object
// from the table, and the table itself "forgets" the object, but it is not
// destroyed.

		virtual Object *Fetch (int pos = 0) = 0;
		virtual int Remove (int pos=0)
			{ Object *o = Fetch (pos); delete o; return o == 0 ? 0 : 1; }

// Search function

		virtual int Search (Object &) ;

// Enquiry functions

		virtual int CurInd () { return curind; }
		virtual int Nelem () { return nelem; }
		char *Type () { return "Table"; }
		enum TblType TblType () { return type; }

// Equality Operator for tables

		int operator == (Object &o) ;

	protected:

		int nelem, curind;
		enum TblType type;
		void Init () { nelem = 0; curind = -1; }
};

class Array : public Table
{
	public:

// Constructor

		Array (int sz=100, enum TblType t = PERS);

// Destructor

		~Array ();

// Insertion functions

		int Append (Object *p);
		int Insert (Object *, int pos=0);

// Overwrite (assign) function

		virtual int Assign (Object *o, int pos = 0);

// Fetch functions

		Object *Get (int pos) ;
		Object *Fetch (int pos=0);

// Enquiry function

		int Size () { return size; }
		char *Type () { return "Array"; }

	protected:

		Object **array;
		int size;
};

class SpArray : public Array
{
	public:

// Constructor

	  SpArray (int sz=100, enum TblType t = PERS) : Array (sz, t) {}

// Destructor - use array destructor.

		~SpArray ();

// Overwrite (assign) function

		virtual int Assign (Object *o, int pos = 0);

// Fetch functions

		Object *Get (int=0) ;
		Object *Fetch (int=0);

//	Search function

		int Search (Object &) ;

// Enquiry function

		char *Type () { return "SpArray"; }
		int IsEmptySlot (int pos) { return ((Get (pos) == 0) ? 1 : 0); }

	private :

// Insertion functions dont make sense for a sparse array - hence hidden away

		int Append (Object *) { return 0; }
		int Insert (Object *, int=0) { return 0; }
};



#define __TBL__
#endif
