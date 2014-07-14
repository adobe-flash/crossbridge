/* File : example.i */
%module ExampleModule

%inline %{
extern int    gcd(int x, int y);
extern double Foo;
%}
