/* APPLE LOCAL file __block assign sequence point 6639533 */
/* { dg-options "-Wall" } */
/* { dg-do compile } */

int foo() {
    __block int retval;
    retval = 0;
    return retval;
}
