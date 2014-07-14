// Reverse Polish Notation, rpn.k
// © 2001, Michael Piefel <piefel@informatik.hu-berlin.de>

// Define which views (ie., different paths) we intent to take
// during unparse and rewrite
%uview infix postfix;
%rview canon calculate;


// Simple expressions
Plus( exp1, exp2 )
	->	[ infix: exp1 "+" exp2 ]
		[ postfix: exp1 " " exp2 " +" ];

Minus( exp1, exp2 )
	->	[ infix: exp1 "-" exp2 ]
		[ postfix: exp1 " " exp2 " -" ];

// Beware of parentheses in infix representation
Mul( exp1, exp2=Plus(*,*) ),
Mul( exp1, exp2=Minus(*,*) )
	->	[ infix: exp1 "*(" exp2 ")" ];

Mul( exp1=Plus(*,*), exp2 ),
Mul( exp1=Minus(*,*), exp2 )
	->	[ infix: "(" exp1 ")*" exp2 ];

Mul( exp1=Plus(*,*), exp2=Plus(*,*) ),
Mul( exp1=Plus(*,*), exp2=Minus(*,*) ),
Mul( exp1=Minus(*,*), exp2=Plus(*,*) ),
Mul( exp1=Minus(*,*), exp2=Minus(*,*) )
	->	[ infix: "(" exp1 ")*(" exp2 ")" ];

Mul( exp1, exp2 )
	->	[ infix: exp1 "*" exp2 ]
		[ postfix: exp1 " " exp2 " *"];

Div( exp1=Plus(*,*), exp2 ),
Div( exp1=Minus(*,*), exp2 )
	->	[ infix: "(" exp1 ")/" exp2 ];

Div( exp1, exp2=Plus(*,*) ),
Div( exp1, exp2=Minus(*,*) ),
Div( exp1, exp2=Mul(*,*) ),
Div( exp1, exp2=Div(*,*) )
	->	[ infix: exp1 "/(" exp2 ")"];

Div( exp1=Plus(*,*), exp2=Plus(*,*) ),
Div( exp1=Plus(*,*), exp2=Minus(*,*) ),
Div( exp1=Minus(*,*), exp2=Plus(*,*) ),
Div( exp1=Minus(*,*), exp2=Minus(*,*) )
	->	[ infix: "(" exp1 ")/(" exp2 ")" ];

Div( exp1, exp2 )
	->	[ infix: exp1 "/" exp2 ]
		[ postfix: exp1 " " exp2 " /" ];


// Calculate all that can be calculated (ie. all where
// we have concrete numbers)
Plus( Term(Number(a)), Term(Number(b)) )
	-> < calculate: Term(Number(plus(a,b)))>;

Plus( Term(Number(a)), Plus(Term(Number(b)), rest) )
	-> < calculate: Plus(Term(Number(plus(a,b))), rest)>;

Plus( Term(Number(a)), Minus(Term(Number(b)), rest) )
	-> < calculate: Minus(Term(Number(plus(a,b))), rest)>;

Minus( Term(Number(a)), Term(Number(b)) )
	-> < calculate: Term(Number(minus(a,b)))>;

Minus( Term(Number(a)), Minus(Term(Number(b)), rest) )
	-> < calculate: Plus(Term(Number(minus(a,b))), rest)>;

Minus( Term(Number(a)), Plus(Term(Number(b)), rest) )
	-> < calculate: Minus(Term(Number(minus(a,b))), rest)>;

Mul( Term(Number(a)), Term(Number(b)) )
	-> < calculate: Term(Number(mul(a,b)))>;

Mul( Term(Number(a)), Mul(Term(Number(b)), rest) )
	-> < calculate: Mul(Term(Number(mul(a,b))), rest)>;

Mul( Term(Number(a)), Div(Term(Number(b)), rest) )
	-> < calculate: Div(Term(Number(mul(a,b))), rest)>;

Div( Term(Number(a)), Term(Number(b)) )
	-> < calculate: Term(Number(divi(a,b)))>;

Div( Term(Number(a)), Div(Term(Number(b)), rest) )
	-> < calculate: Mul(Term(Number(divi(a,b))), rest)>;

Div( Term(Number(a)), Mul(Term(Number(b)), rest) )
	-> < calculate: Div(Term(Number(divi(a,b))), rest)>;

// Helper functions to actually compute
%{ KC_REWRITE
inline integer plus(integer a, integer b) { return mkinteger(a->value+b->value); }
inline integer minus(integer a, integer b) { return mkinteger(a->value-b->value); }
inline integer mul(integer a, integer b) { return mkinteger(a->value*b->value); }
inline integer divi(integer a, integer b) { return mkinteger(b->value==0 ? 0 : a->value / b->value); }
%}


// Rewrite to a canonical form of the expression where operators
// are to be put as far to the end as possible
Plus( Plus(a, b), c)
	-> < canon: Plus(a, Plus(b, c))>;

Plus( Minus(a, b), c)
	-> < canon: Plus(c, Minus(a, b))>;

Minus( Plus(a, b), c)
	-> < canon: Plus(a, Minus(b, c))>;

Mul( Mul(a, b), c)
	-> < canon: Mul(a, Mul(b, c))>;

Plus( a=Term(Ident(*)), b=Term(Number(*)) )
	-> < canon: Plus(b, a)>;

Mul( a=Term(Ident(*)), b=Term(Number(*)) )
	-> < canon: Mul(b, a)>;

Plus( a=Term(Ident(*)), Plus(b=Term(Number(*)), rest) )
	-> < canon: Plus(b, Plus(a,rest))>;

Mul( a=Term(Ident(*)), Mul(b=Term(Number(*)), rest) )
	-> < canon: Mul(b, Mul(a,rest))>;

