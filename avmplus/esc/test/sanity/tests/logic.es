print (f() || g())  // true true
print (g() || f())  // false true true
print (f() && g())  // true false false
print (g() && f())  // false false
function f() { print (true); return true }
function g() { print (false); return false }