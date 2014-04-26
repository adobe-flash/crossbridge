// LLVM LOCAL file
// { dg-do compile }
// { dg-options "" }
// PR 1805

int baz(void*);

int foo() {
 static void* bar[] = { &&bb1, &&bb2, &&bb3 };
 switch (baz(bar)) {
 case 1:
   goto bb1;
 case 2:
   goto bb2;
 default:
   goto bb3;
 }
 bb1: return 1;
 bb2: return 2;
 bb3: return 3;
}
