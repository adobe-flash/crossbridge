/* APPLE LOCAL file 5109856 */
/* { dg-do compile } */
/* { dg-options "-O2" } */
class BaseLong {
protected:
  union
 {
    double double_value;
  } value;
} ;
class IEEEdouble :
  public BaseLong {
  inline IEEEdouble(double d) {value.double_value = d;}
  IEEEdouble();
} ;
extern bool bigger(IEEEdouble, IEEEdouble);
IEEEdouble::IEEEdouble() {
  IEEEdouble aadj;
    while (true)
       if (bigger(aadj, IEEEdouble(1))) ;
}
