/* APPLE LOCAL file KEXT double destructor */
/* { dg-do compile } */
/* { dg-options "-fapple-kext" } */

class tTest
{
    public:

    tTest();
    ~tTest();
};

void bar ()
{
    tTest tests[5];
}
