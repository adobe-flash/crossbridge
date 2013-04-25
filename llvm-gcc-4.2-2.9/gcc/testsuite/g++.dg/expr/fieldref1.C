// APPLE LOCAL file - test of access to 8-byte struct field
// Radar 3309305: positive C++ test case
// Origin: Matt Austern <austern@apple.com>
// { dg-do run }

struct X {
   char array[8];
};

char* get_array(X* p) {
  char* p2 = p->array;
  return p2;
}

int main()
{
  X t;
  X* p = &t;
  char* p2 = get_array(p);

  bool ok = (void*)p == (void*)p2;
  return !ok;
}
