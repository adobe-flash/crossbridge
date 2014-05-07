/* File : example.i */
#ifdef SWIG
%module ExampleModule
#endif

struct Bar {
    int a;
    unsigned b;
    char c;
    double d;
    int nums[4];
};

extern void set_struct_members(struct Bar *bar);
extern void print_struct_members(struct Bar *bar);
extern void print_struct_offsets();

