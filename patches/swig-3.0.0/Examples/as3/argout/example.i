/* File : example.i */
%module ExampleModule

%typemap(in, numinputs=0) (int *x, int *y) (int tempx, int tempy) {
    $1 = &tempx;
    $2 = &tempy;
}

%typemap(astype) (void getpoint) "Object";
%typemap(asreturn) (void getpoint) {
    AS3_ReturnAS3Var($input);
}

%typemap(argout) (int *x, int *y) {
    inline_as3("var $result = {'x': CModule.read32(%0), "
               "'y': CModule.read32(%1)};"
                : : "r"($1), "r"($2));
}

%inline %{
extern void getpoint(int *x, int *y);
%}

