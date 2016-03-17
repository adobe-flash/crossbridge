
#ifndef __SIMPLE_H__
#define __SIMPLE_H__

typedef
struct baz {
    
    char b;
    int a;
} sbaz;

int foo;
char *bar();
void print_int(int d);
void print_float(float f);
void print_double(double d);
int return42();
float return42f();
double return42d();
void print_str(char *);
void set_callback(int (*func)());
void set_callback2(int (*funct)(int, double));
void set_callback3(double (*func)());
//void set_callback4(sbaz (*func)());
//void set_callback5(void (*func)(sbaz));
void do_call();


/*
struct fooz;

struct baaz {
    short z;
    struct fooz a;
    char b;
};
*/

struct baaaz {
    short a;
    sbaz b;
};

union biz {
        char z;
        double bar;
};

struct boz {
        char *str;
        long long big;
};

struct fooz {
    short a;
    int *b;
    union biz u;
    struct boz inner;
};

/* this construct is problematic for swig
struct fooz {
    short a;
    int *b;
    
    union biz {
        char z;
        double bar;
    } u;

    struct boz {
        char *str;
        long long big;
    } inner;
};
*/

/*
struct fooz_u {
    union biz a;
    struct boz b;
};
*/

#endif // __SIMPLE_H__
