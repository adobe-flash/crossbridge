// APPLE LOCAL file mainline radar 6194879
// Test for the warning of exposing types from an anonymous namespace
// { dg-do compile }

#include "anonymous-namespace-1.h"

namespace {
 class good { };
}

class foo::bar : public good { }; 
class foobar1
{
  good g;
};

#line 18 "foo.C"
class foobar : public bad { }; // { dg-warning "uses the anonymous namespace" }
class foobar2 { bad b; }; // { dg-warning "uses the anonymous namespace" }
