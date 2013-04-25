/* APPLE LOCAL file AltiVec */
/* { dg-do compile { target powerpc*-*-darwin* } } */
/* { dg-options "-O -faltivec" } */
/* LLVM LOCAL begin check for arbitrary spaces in asm output */
/* { dg-final { scan-assembler "vspltisw v2, *1" } } */
/* { dg-final { scan-assembler "vspltisw v3, *2" } } */
/* { dg-final { scan-assembler "vspltisw v4, *3" } } */
/* { dg-final { scan-assembler "vspltisw v5, *4" } } */
/* { dg-final { scan-assembler "vspltisw v6, *5" } } */
/* { dg-final { scan-assembler "vspltisw v7, *6" } } */
/* { dg-final { scan-assembler "vspltisw v8, *7" } } */
/* { dg-final { scan-assembler "vspltisw v9, *8" } } */
/* { dg-final { scan-assembler "vspltisw v10, *9" } } */
/* { dg-final { scan-assembler "vspltisw v11, *10" } } */
/* { dg-final { scan-assembler "vspltisw v12, *11" } } */
/* { dg-final { scan-assembler "vspltisw v13, *12" } } */
/* LLVM LOCAL end check for arbitrary spaces in asm output */
 
void
foo (vector signed int v0, vector signed int v1, vector signed int v2,
     vector signed int v3, vector signed int v4, vector signed int v5,
     vector signed int v6, vector signed int v7, vector signed int v8,
     vector signed int v9, vector signed int v10, vector signed int v11,
     vector signed int v12,
     int z, double u);

int main(void) 
{
  foo ((vector signed int) ( 1 ), (vector signed int) ( 2 ), 
       (vector signed int) ( 3 ), 
       (vector signed int) ( 4 ), (vector signed int) ( 5 ), 
       (vector signed int) ( 6 ), (vector signed int) ( 7 ), 
       (vector signed int) ( 8 ), (vector signed int) ( 9 ), 
       (vector signed int) ( 10 ), (vector signed int) ( 11 ), 
       (vector signed int) ( 12 ), (vector signed int) ( 13 ),
       20, 3.0);
}

    
		 
		 
