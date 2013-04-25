/* APPLE LOCAL file anon types 6822746 */
/* dg-do compile */

class C {
public:
  C();
  C(const C& o);
};

void foo() {
  typedef struct {
    C m;
  } T;
  T l[4];
}
