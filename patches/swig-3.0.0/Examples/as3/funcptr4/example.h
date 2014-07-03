/* file: example.h */

typedef char *(*str_func)(char *str, int size);

extern char *str;
extern const char *(*char_func)();

extern str_func (*str_func_getter)();

extern void (*str_func_setter)(str_func);

extern void call_getter();
extern void call_str_func();
extern void a_str_func_setter(str_func);

