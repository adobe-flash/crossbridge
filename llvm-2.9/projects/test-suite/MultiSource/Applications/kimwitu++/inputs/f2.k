// Reverse Polish Notation, main.k
// © 2001, Michael Piefel <piefel@informatik.hu-berlin.de>

%{
#include <iostream>
#include "k.h"
#include "rk.h"
#include "unpk.h"
#include "csgiok.h"

int yyparse();
line TheLine;
%}
%{ KC_TYPES_HEADER
extern line TheLine;
%}

// Yes, create YYSTYPE union for the bison parser.
%option yystype

// Trivial printer function (ignores view)
void
printer(const char *s, uview v)
{
	std::cout << s;
}

int
main(int argc, char **argv)
{
	FILE* f;

	std::cout << " RPN Parser and reformatter " << std::endl;
	// If a saved tree is given on command line, read it
	if (argc==2) {
	    f=fopen(argv[1], "r");
	    kc::CSGIOread(f, TheLine);
	    fclose(f);
	} else yyparse();

	line TheCanonLine=TheLine->rewrite(canon);
	line TheShortLine=TheCanonLine->rewrite(calculate);

	std::cout << "\nInfix notation:\n";
	TheCanonLine->unparse(printer, infix);

	std::cout << "\n\nCanonical postfix notation:\n";
	TheCanonLine->unparse(printer, postfix);

	std::cout << "\n\nCalculated infix notation:\n";
	TheShortLine->unparse(printer, infix);

	std::cout << "\n\nCalculated canonical postfix notation:\n";
	TheShortLine->unparse(printer, postfix);

	std::cout << std::endl;
}

