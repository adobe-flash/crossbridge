/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// Sundry ad-hoc test cases.
// All the functions here print a single line of output containing PASSED if it passed and FAILED if not.

// Missing tests:
//  - static initializers (eval does not support classes yet)
//  - class methods (eval does not support classes yet)
//  - configurable groups (at various levels)

// Harness

var out = "";
function pr(x) { out += x }
function prln(tag) { if (out == "0123456789") print(tag + ": PASSED"); else print(tag + ": FAILED: " + out); out=""; }

// Simple loop, goto on same level inside function

function fa()
{
	var i=0;
L1: pr(i);
	i++;
	if (i < 10) goto L1;
	prln("fa");
}
fa();


// Goto nested in an inner block

function fb()
{
	var i=0;
L1: pr(i);
	i++;
	if (i < 10) {
		var j=i;
		goto L1;
	}
	prln("fb");
}
fb();

// Ditto, but multiple labels on a statement

function fc()
{
	var i=0;
L0: L1: L2:
	pr(i);
	i++;
	if (i < 10) {
		var j=i;
		goto L1;
	}
	prln("fc");
}
fc();

// Goto a nesting label that's not "top level" in any block

function fd()
{
	var i=0;
	if (!(i & 255))
	L1: if (i < 10) {
			pr(i);
			i++;
			goto L1;
		}
	prln("fd");
}
fd();

// Goto inside a switch (and also some forward jumps)

function fe()
{
	switch (1) {
	case 0:
	L0: pr("1");
		goto L2;
	case 1:
		pr("0");
		goto L0;
	L2:
		pr("23456789");
		break;
	default:
		goto L2;
	}
	prln("fe");
}
fe();

// Unambiguous label because the supposed ambiguity is in a nested inaccessible scope

function ff()
{
	var i=0;
	{ L1: pr(i); i++; }
L1: pr(i); 
	i++;
	if (i < 10) goto L1;
	prln("ff");
}
ff();

// Unambiguous L1 and L2 in the inner function because the supposed ambiguity is outside the function

function fg()
{
	function inner()
	{
		var i=0;
	L1: pr(i);
		i++;
		if (i == 10) goto L2;
		goto L1;
	L2: prln("fg");
	}
L1: L2:
	inner();
}
fg();

// Goto inside a switch.  Note L0 precedes the case label for case 0,
// we're checking that we fall through correctly.  Case -1 is
// synthetic, the language grammar requires it (we can't start the
// case list with a statement).  It may be that with the introduction
// of goto in the language, it /is/ reasonable to allow the case list
// to start with a statement.  C#, which has "goto", does not allow
// that, though it has "goto case" to make up for it.

function fh()
{
	switch (1) {
	case -1:
	L0:;
	case 0:
	    pr("1");
		goto L2;
	case 1:
		pr("0");
		goto L0;
	L2:
		pr("23456789");
		break;
	default:
		goto L2;
	}
	prln("fh");
}
fh();

// Jump out past "finally"

function fi()
{
	try {
		try {
			goto L1;
		}
		finally {
			pr("0");
		}
	}
	finally {
		pr("1");
	}
L1: pr("23456789");
	prln("fi");
}
fi();

// Top level code

// t0

var c = 0;
L1: 
if (c == 10) goto L2;
pr(c);
c++;
goto L1;
L2:
prln("t0");

// Two top level groups that should not confuse each others'
// identically named labels (though note that adding L3 or L4 at the
// top level will introduce syntax errors here, see goto-neg5.as).

//t1a
{
	var d = 0;
L3:	if (d == 10) goto L4;
	pr(d);
	d++;
	goto L3;
L4:	prln("t1a");
}

//t1b
{
	var e = 0;
L3:	if (e == 10) goto L4;
	pr(e);
	e++;
	goto L3;
L4:	prln("t1b");
}
