/* APPLE LOCAL file radar 6246527 */
/* { dg-do compile } */
/* { dg-options "-Wformat" } */
#include <stddef.h>

void (^e) (int arg, const char * format, ...) __attribute__ ((__sentinel__ (1,1)));
void (^e1) (int arg, const char * format, ...) __attribute__ ((__sentinel__ (1,1, 3))); /* { dg-error "wrong number of arguments" "sentinel" } */

int main()
{
        void (^b) (int arg, const char * format, ...) __attribute__ ((__sentinel__)) = 
						^ __attribute__ ((__sentinel__)) (int arg, const char * format, ...) {};
        void (^z) (int arg, const char * format, ...) __attribute__ ((__sentinel__ (2))) = ^ __attribute__ ((__sentinel__ (2))) (int arg, const char * format, ...) {};


        void (^y) (int arg, const char * format, ...) __attribute__ ((__sentinel__ (5))) = ^ __attribute__ ((__sentinel__ (5))) (int arg, const char * format, ...) {};

	b(1, "%s", NULL);	// OK
	b(1, "%s", 0); /* { dg-warning "missing sentinel in function call" } */                                                         	
	z(1, "%s",4 ,1,0); /* { dg-warning "missing sentinel in function call" } */
	z(1, "%s", NULL, 1, 0);	// OK

	y(1, "%s", 1,2,3,4,5,6,7); /* { dg-warning "missing sentinel in function call" } */

	y(1, "%s", NULL,3,4,5,6,7);	// OK

}

