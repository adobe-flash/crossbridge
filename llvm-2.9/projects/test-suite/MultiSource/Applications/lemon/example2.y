/* Copyright (GPL) 2004 mchirico@users.sourceforge.net or mchirico@comcast.net
  Simple lemon parser  example.
  
  Download:
  http://prdownloads.sourceforge.net/souptonuts/lemon_examples.tar.gz?download


  To compile this example see the Makefile. Or
  this is what is taking place.
  
    $ ./lemon example2.y                          
    $ cat main_part2 >> example2.c
    $ g++ -o ex2  -O2 -s -pipe example2.c

  Next, run ./ex2

    $ ./ex2

  Which will give the following output:

    Result.value=17  <---------------------------\       
    Result.n=4        				 |
    Result.value=-9   				 |
    Result.n=4        				 |
    Result.value=78   				 |
    Result.n=10       				 |
						 |
  Now looking at a section of main_part2	 |
  we can see how 4 PLUS 13 in implemented.	 |
						 |
      struct Token t0,t1;              		 |
      struct Token mToken;         		 |
                                   		 |
      t0.value=4;                  		 |
      t0.n=0;                      		 |
                                   		 |
      t1.value=13;                               |
      t1.n=0;                                    |
         // Below 4 PLUS 14    ----------------- /                         
      Parse (pParser, NUM, t0);    
      Parse (pParser, PLUS, t0);   
      Parse (pParser, NUM, t1);    
      Parse (pParser, 0, t0);      









*/

%include {   
#include <iostream>  
#include "ex2def.h"
#include "example2.h"
}  


%token_type {Token}
%default_type {Token}


%type expr {Token}
%type NUM {Token}
   
%left PLUS MINUS.   
%left DIVIDE TIMES.  
   

   
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

