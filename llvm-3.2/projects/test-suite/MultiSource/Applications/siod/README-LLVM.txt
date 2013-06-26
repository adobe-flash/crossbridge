
WHAT IS THIS?
=============

This is SIOD ("Scheme in One Defun, but in C this time") by George
J. Carrette, retrieved from the freebsd distfiles collection
on 1-Oct-2003.


WHERE IS THE DOCUMENTATION?
===========================

http://people.delphiforums.com/gjc/siod.html


WHAT CHANGES HAVE BEEN MADE?
============================

I removed all the files except for those that need to be compiled to get
the thing to work, and gave it a small MultiSource-friendly Makefile. I
also #ifdefed out the "Evaluation took ... seconds" message that gets
printed out after every top-level REPL expression, in slib.c (search for
"ifdef STATISTICS").

We also renamed lchmod to l_chmod, ripped out lchown() and lcrypt(),
and the reference to putpwent(). We provided a default definition for
PATH_MAX.


WHAT ELSE NEEDS TO BE DONE?
===========================

Add some interesting test code to test.scm. Right now it's really
boring stuff.

Once invoke is better supported we should exercise it, e.g.,
we would want to add a test that tries something like this:

	> (*catch 'foo (*throw 'foo 1))
	1
	> (*catch 'foo (*throw 'bar 1))
	ERROR: no *catch found with this tag (errobj bar)


-Brian
Wed Oct  1 19:19:00 CDT 2003
