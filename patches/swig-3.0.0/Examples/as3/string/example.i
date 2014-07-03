/* File : example.i */
%module ExampleModule

%inline %{
extern void    print_str(char *str);
%}
