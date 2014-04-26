/* APPLE LOCAL file radar 4430340 */
/* { dg-do compile } */
int a[1/0]; /* { dg-warning "division by zero" } */
            /* APPLE LOCAL radar 4859613 */
            /* { dg-error "error: variably modified 'a' at file scope" "variably" { target *-*-* } 3 } */
/* APPLE LOCAL file radar 4430340 */
