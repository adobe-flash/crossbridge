// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
//  Written by Hemant Pande, Rutgers University.  December 1994.  May be
//  distributed freely, provided this comment is displayed at the top.
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include <cstdio>

class True {
public:
 True() {}
 virtual True *and_m (True *arg)
 {
   printf("T\n");
   return arg;
 }
} tru;

class False : public True {
public:
  False() {}
  True *and_m (True *arg);
} fals;

True *False::and_m (True *arg)
{
   printf("F\n");
   return &fals;
}

True *v1, *nv1, *v2, *nv2, *v3, *nv3;
True *c;

int main(int argc, char **argv) {
   if (0) {v1 = &tru; nv1 = &fals;} else {v1 = &fals; nv1= &tru;}
   if (0) {v2 = &tru; nv2 = &fals;} else {v2 = &fals; nv2 = &tru;}
   if (0) {v3 = &tru; nv3 = &fals;} else {v3 = &fals; nv3 = &tru;}
   if (0) c = nv2; else if (0) c = v1; else c = v2;
   if (0) c = c->and_m(nv3); else if (0) c = c->and_m(nv2); else c = c->and_m(v1);
   if (0) c = c->and_m(v2); else if (0) c = c->and_m(nv1); else c = c->and_m(v3);
}
