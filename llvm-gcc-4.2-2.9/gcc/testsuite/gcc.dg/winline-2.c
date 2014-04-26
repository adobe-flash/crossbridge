/* { dg-do compile } */
/* { dg-options "-Winline -O2" } */

/* APPLE LOCAL wording 4598393 */
inline int q(void);		 /* { dg-warning "the function body must appear before caller" "" } */
inline int t(void)
{
	return q();		 /* { dg-warning "called from here" "" } */
}
