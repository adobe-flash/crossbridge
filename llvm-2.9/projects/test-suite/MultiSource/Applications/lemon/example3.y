/* Copyright (GPL) 2004 mchirico@users.sourceforge.net or mchirico@comcast.net
  Simple lemon parser  example.

  
    $ ./lemon example2.y                          
    $ cat main_part2 >> example2.c
    $ g++ -o ex2  -O2 -s -pipe example2.c

  Now, to run the example.

    $ ./ex2
    
  The output will be as follows:
 
    
    Result.value=17 <-------------------
    Result.n=4          		|
    Result.value=-9     		|
    Result.n=4          		|
    Result.value=78     		|
    Result.n=10         		|
					|
  Take a look at main_part2		|
					|
					|
      t0.value=4;                       | 
      t0.n=0;                        	|
                                        |
      t1.value=13;                   	|
      t1.n=0;                        	|
                                     	|
      //Note below is 4 PLUS 17   ------
      Parse (pParser, NUM, t0);      	
      Parse (pParser, PLUS, t0);     	
      Parse (pParser, NUM, t1);      	
      Parse (pParser, 0, t0);        	
					
*/					
					
%include {   
#include <iostream>  
#include "ex3def.h"
#include "example3.h"

  void token_destructor(Token t)
    {
      std::cout << "In token_destructor t.value= " << t.value << std::endl;
      std::cout << "In token_destructor t.n= " << t.n << std::endl;
    }

}  

%token_type {Token}
%default_type {Token}
%token_destructor { token_destructor($$); }

%type expr {Token}
%type id {Token}
   
%left PLUS MINUS.   
%left DIVIDE TIMES.  
   
%parse_accept {
  printf("parsing complete!\n\n\n");
}
   
%syntax_error {  
  std::cout << "Syntax error!" << std::endl;  
}   
   
program ::= expr(A).   { 
                        std::cout << "Result.value=" << A.value << std::endl; 
                        std::cout << "Result.n=" << A.n << std::endl; 

                         }  

expr(A) ::= expr(B) MINUS  expr(C).   { A.value = B.value - C.value; 
                                       A.n = B.n+1  + C.n+1;
                                      }  

expr(A) ::= expr(B) PLUS  expr(C).   { A.value = B.value + C.value; 
                                       A.n = B.n+1  + C.n+1;
                                      }  

expr(A) ::= expr(B) TIMES  expr(C).   { A.value = B.value * C.value;
                                        A.n = B.n+1  + C.n+1;

                                         }  
expr(A) ::= expr(B) DIVIDE expr(C).  { 

         if(C.value != 0){
           A.value = B.value / C.value;
           A.n = B.n+1 + C.n+1;
          }else{
           std::cout << "divide by zero" << std::endl;
           }
}  /* end of DIVIDE */
expr(A) ::= NUM(B). { A.value = B.value; A.n = B.n+1; }


