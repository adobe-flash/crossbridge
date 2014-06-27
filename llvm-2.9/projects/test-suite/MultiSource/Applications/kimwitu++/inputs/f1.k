// Reverse Polish Notation, abs.k
// © 2001, Michael Piefel <piefel@informatik.hu-berlin.de>

// The abstract syntax of our expressions
line:
	Line(expression);

term:
	Number(integer) | Ident(casestring);

expression:
	Term( term )
	| Plus( expression expression )
	| Mul( expression expression )
	| Minus( expression expression )
	| Div( expression expression )
	| Div2( expression casestring expression )
	;


