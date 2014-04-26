/* APPLE LOCAL begin radar 4278236 */
class TestCPP { }; 	/* { dg-error "previous declaration of" } */

@class TestCPP;		/* { dg-error "redeclared as different kind of symbol" } */
/* APPLE LOCAL end radar 4278236 */
