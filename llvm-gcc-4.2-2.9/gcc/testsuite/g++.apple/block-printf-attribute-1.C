/* APPLE LOCAL file radar 6246527 */
/* { dg-do compile } */
/* { dg-options "-Wformat" } */

int main()
{
        void (^b) (int arg, const char * format, ...) __attribute__ ((__format__ (__printf__, 1, 3))) =  /* { dg-error "format string argument not a string type" } */
						^ __attribute__ ((__format__ (__printf__, 1, 3))) (int arg, const char * format, ...) {}; /* { dg-error "format string argument not a string type" } */
        void (^z) (int arg, const char * format, ...) __attribute__ ((__format__ (__printf__, 2, 3))) = ^ __attribute__ ((__format__ (__printf__, 2, 3))) (int arg, const char * format, ...) {};

	z(1, "%s", 1); /* { dg-warning "format \\'\%s\\' expects type" } */
	z(1, "%s", "HELLO");

}

