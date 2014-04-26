// { dg-do run  }
// Copyright (C) 2000 Free Software Foundation, Inc.
// Contributed by Nathan Sidwell 16 Jan 2001 <nathan@codesourcery.com>

// Bug 1611. Under the new ABI, the vtable can be clobbered during dtoring our
// primary vbase. We mustn't use the vtable after that to locate our vbases.

#if defined (__GXX_ABI_VERSION) && __GXX_ABI_VERSION >= 100
#include <stdio.h>
#include <stdlib.h>

int *ctorVBase = 0;
int *dtorVBase = 0;
int *ctorVDerived = 0;
int *dtorVDerived = 0;
int *ctorB = 0;
int *dtorB = 0;

struct VBase
{
  int member;
  VBase ()
    {
      if (ctorVBase) exit (1);
      ctorVBase = &member;
    }
  virtual ~VBase ()
    {
      if (dtorVBase) exit (2);
      dtorVBase = &member;
      if (dtorVBase != ctorVBase) exit (3);
    }
  void Offset () const
  {
    printf ("VBase\n");
    /* APPLE LOCAL default to Wformat-security 5764921 */
    printf ("  VBase::member %d\n", int(&this->VBase::member - (int *)this));
  }
};

struct VDerived : virtual VBase
{
  int member;
  
  VDerived ()
    {
      if (ctorVDerived) exit (4);
      ctorVDerived = &member;
    }
  virtual ~VDerived ()
    {
      if (dtorVDerived) exit (5);
      dtorVDerived = &member;
      if (dtorVDerived != ctorVDerived) exit (6);
    }
  void Offset () const
  {
    printf ("VDerived\n");
    /* APPLE LOCAL begin default to Wformat-security 5764921 */
    printf ("  VBase::member %d\n", int(&this->VBase::member - (int *)this));
    printf ("  VDerived::member %d\n", int(&this->VDerived::member - (int *)this));
    /* APPLE LOCAL end default to Wformat-security 5764921 */
  }
};
struct B : virtual VBase
{
  int member;
  void Offset () const
  {
    printf ("B\n");
    /* APPLE LOCAL begin default to Wformat-security 5764921 */
    printf ("  VBase::member %d\n", int(&this->VBase::member - (int *)this));
    printf ("  B::member %d\n", int(&this->B::member - (int *)this));
    /* APPLE LOCAL end default to Wformat-security 5764921 */
  }
};
struct MostDerived : B, virtual VDerived
{
  int member;
  void Offset () const
  {
    printf ("MostDerived\n");
    /* APPLE LOCAL begin default to Wformat-security 5764921 */
    printf ("  VBase::member %d\n", int(&this->VBase::member - (int *)this));
    printf ("  B::member %d\n", int(&this->B::member - (int *)this));
    printf ("  VDerived::member %d\n", int(&this->VDerived::member - (int *)this));
    printf ("  MostDerived::member %d\n", int(&this->MostDerived::member - (int *)this));
    /* APPLE LOCAL end default to Wformat-security 5764921 */
  }
};


int main ()
{
  {
    MostDerived dum;
    
    int *this_ = (int *)&dum;
    
    if (ctorVBase != &dum.VBase::member)
      return 23;
    if (ctorVDerived != &dum.VDerived::member)
      return 24;
    
    /* APPLE LOCAL begin default to Wformat-security 5764921 */
    printf ("  VBase::member %d\n", int(&dum.VBase::member - this_));
    printf ("  B::member %d\n", int(&dum.B::member - this_));
    printf ("  VDerived::member %d\n", int(&dum.VDerived::member - this_));
    printf ("  MostDerived::member %d\n", int(&dum.MostDerived::member - this_));
    /* APPLE LOCAL end default to Wformat-security 5764921 */
    dum.MostDerived::Offset ();
    dum.B::Offset ();
    dum.VDerived::Offset ();
    dum.VBase::Offset ();
  }
  return 0;
}
#else /* !(defined (__GXX_ABI_VERSION) && __GXX_ABI_VERSION >= 100) */

int main () 
{
}

#endif /* !(defined (__GXX_ABI_VERSION) && __GXX_ABI_VERSION >= 100) */
