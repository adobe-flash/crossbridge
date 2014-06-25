/* File : example.i */
%module ExampleModule

%typemap(in) (int len, char *str) {
    AS3_MallocString($2, $input);
    AS3_StringLength($1, $input);
};

%typemap(astype) (int len, char *str) "String";

%typemap(freearg) (int len, char *str) {
    printf("cleaning up\n");
    free($2);
};

%inline %{
extern void print_str(int len, char *str);
%}
