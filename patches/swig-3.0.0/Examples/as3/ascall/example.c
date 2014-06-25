/* File : example.c */
static int (*callback)();
static int (*callback2)(int, double);
static double (*callback3)();

void set_callback(int (*func)()) {
    callback = func;
}

void set_callback2(int (*funct)(int, double)) {
    callback2 = funct;
}

void set_callback3(double (*func)()) {
    callback3 = func;
}

void do_call() {
    int i = callback();
    printf("callback returned %d to c\n", i);
    int j = callback2(123, 8.76);
    printf("callback2 returned %d to c\n", j);
    double d = callback3();
    printf("callback3 returned %f to c\n", d);
}

